<?php

/**
 * @file
 *  Drush integration.
 */

/**
 * Implements hook_drush_command().
 */
function intercomio_drush_command() {
  $items['intercomio-update-users'] = array(
    'description' => dt('Sends all the users to intercomio.'),
    'arguments' => array(
      'uids' => 'A comma-separated list of user ids. If empty, then all the users will be sent.'
    ),
    'examples' => array(
      'drush intercomio-update-users' => 'Sends all the users to intercomio.',
      'drush intercomio-update-users 567,789' => 'Sends the users witht he 567 and 789 to intercomio.'
    ),
  );

  return $items;
}

function drush_intercomio_update_users($uids = NULL) {
  // Load all the users which are active.
  $user_ids = !empty($uids) ? explode(',', $uids) : array();
  array_walk($user_ids, 'trim');

  $query = db_select('users', 'u')->fields('u', array('uid'));
  if (!empty($user_ids)) {
    $query->condition('u.uid', $user_ids);
  }
  $query->condition('u.uid', 0, '>')->condition('u.status', 1);
  $result = $query->execute()->fetchAll();

  $total = count($result);
  $index = 0;
  $succeded = 0;
  $app_id = variable_get('intercomio_app_id', '');
  $api_key = variable_get('intercomio_api_key', '');

  foreach ($result as $row) {
    $index++;
    $account = user_load($row->uid);

    // Step 1: prepare the data to be sent.
    $data = array(
      'user_id' => $account->uid,
      'email' => $account->mail,
      'created_at' => $account->created,
      'name' => format_username($account),
      'new_session' => FALSE,
    );

    $custom_data = array();
    $custom_data += module_invoke_all('intercomio_custom_data', variable_get('intercomio_custom_data', array()));
    if (!empty($custom_data)) {
      $data['custom_data'] = $custom_data;
    }

    if (variable_get('intercomio_secure') && $secure_key = variable_get('intercomio_secure_key')) {
      $data['user_hash'] = sha1($secure_key . $account->uid);
    }
    // End step 1.

    // Step 2: prepare the curl options.
    $curl_options = array(
      CURLOPT_URL => 'https://api.intercom.io/users',
      CURLOPT_POST => TRUE,
      CURLOPT_TIMEOUT => 2, // do not wait more than 2 sec to connect
      CURLOPT_POSTFIELDS => json_encode($data),
      CURLOPT_HTTPHEADER => array('Content-Type: application/json'),
      CURLOPT_HTTPAUTH => CURLAUTH_BASIC,
      CURLOPT_USERPWD => "$app_id:$api_key",
      CURLOPT_RETURNTRANSFER => TRUE,
    );
    // End step 2.

    // Step 3: perform the request and log the data.
    try {
      $curl_handler = curl_init();
      curl_setopt_array($curl_handler, $curl_options);
      $response = curl_exec($curl_handler);
      if (!$response) {
        $args = array(
          '@errno' => curl_errno($curl_handler),
          '@error' => curl_error($curl_handler),
          '@mail' => $account->mail,
        );
        drush_log(dt('Failed to send Intercom.io user @mail with error #@errno: @error', $args), 'error');
      }
      else {
        $status_code = curl_getinfo($curl_handler, CURLINFO_HTTP_CODE);
        if ($status_code != 200) {
          // We just print the errors on the screen.
          drush_log(dt('There was an error trying to send the user @mail to Intercomio: !error', array('@mail' => $account->mail, '!error' => $response)), 'error');
        }
        else {
          $succeded++;
          drush_log(dt('User @mail sent to Intercomio.', array('@mail' => $account->mai)), 'success');
        }
      }

      curl_close($curl_handler);
    }
    catch(Exception $e) {
      drush_log($e->getMessage());
    }

    // After every 20 request, we stop for 5 seconds.
    if ($index % 20 == 0) {
      $percent = number_format(($index / $total) * 100, 2);
      drush_log(dt('!count users from !total processed (!percent  %).', array('!count' => $index, '!total' => $total, '!percent' => $percent)), 'success');
      sleep(5);
    }
    // End step 3.
  }
  drush_log(dt('!count users processed. !succedded succedded.', array('!count' => $index, '!succedded' => $succeded)), 'success');
}
