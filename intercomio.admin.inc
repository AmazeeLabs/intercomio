<?php

function intercomio_admin_form() {
  $form = array();

  $form['intercomio_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable intercom.io tracking'),
    '#default_value' => variable_get('intercomio_enabled', TRUE),
  );

  $form['intercomio_app_id'] = array(
    '#type' => 'textfield',
    '#title' => t('App ID'),
    '#description' => t('Enter your Intercom.io app id here.'),
    '#default_value' => variable_get('intercomio_app_id'),
  );

  $form['intercomio_access_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Access token'),
    '#description' => t('Enter the Intercomio access token.'),
    '#default_value' => variable_get('intercomio_access_token'),
  );

  $form['intercomio_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#description' => t('API key with write permission used to trigger events on page load') . '. ' . t('<em>Deprecated, use the access token instead.</em>'),
    '#default_value' => variable_get('intercomio_api_key'),
  );

  $form['intercomio_role_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Roles'),
  );

  $form['intercomio_role_settings']['intercomio_visibility_roles'] = array(
    '#type' => 'radios',
    '#title' => t('Add intercom.io for specific roles'),
    '#options' => array(
      t('Add to the selected roles only'),
      t('Add to every role except the selected ones'),
    ),
    '#default_value' => variable_get('intercomio_visibility_roles'),
  );

  $form['intercomio_role_settings']['intercomio_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles to track'),
    '#description' => t('Select all of the roles you would like to track. Leave empty to select all roles.'),
    '#options' => user_roles(TRUE),
    '#default_value' => variable_get('intercomio_roles', array()),
  );

  $form['intercomio_region_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Region'),
  );

  $form['intercomio_region_settings']['intercomio_region'] = array(
    '#type' => 'select',
    '#title' => t('Region'),
    '#options' => system_region_list(variable_get('theme_default'), $show = REGIONS_ALL),
    '#default_value' => variable_get('intercomio_region'),
  );

  $form['intercomio_secure_mode'] = array(
    '#type' => 'fieldset',
    '#title' => t('Secure mode'),
  );
  
  $form['intercomio_secure_mode']['intercomio_secure'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use secure mode key'),
    '#description' => t('After enabling secure mode on your site, you must also remember to enable it within intercom.io itself. See <a href="!url">http://docs.intercom.io/#SecureMode</a> for further information.', array('!url' => 'http://docs.intercom.io/#SecureMode')),
    '#default_value' => variable_get('intercomio_secure', FALSE),
  );

  $form['intercomio_secure_mode']['intercomio_secure_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Intercom secure key'),
    '#description' => t('Find this on the same page at intercom.io where you enable <em>secure mode</em>. In the example code the final line contains a sha1 generation function which concatenates the secure key with the users email address. Paste that secure key here.'),
    '#default_value' => variable_get('intercomio_secure_key'),
  );

  $form['intercomio_inbox'] = array(
    '#type' => 'fieldset',
    '#title' => t('Inbox'),
  );

  $form['intercomio_inbox']['intercomio_inbox_type'] = array(
    '#type' => 'select',
    '#title' => t('Inbox type'),
    '#options' => array(
      0 => t('None'),
      'simple' => t('Simple'),
      'advanced' => t('Advanced'),
    ),
    '#default_value' => variable_get('intercomio_inbox_type'),
  );
  $form['intercomio_inbox']['intercomio_inbox_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Inbox label'),
    '#description' => t('This is only used with the <em>Simple</em> inbox type.'),
    '#default_value' => variable_get('intercomio_inbox_label'),
  );

  $form['intercomio_custom_data'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Include the following custom data settings'),
    '#options' => intercomio_custom_data_settings(),
    '#default_value' => variable_get('intercomio_custom_data', array()),
  );

  $form = system_settings_form($form);
  $form['#submit'][] = 'intercomio_admin_form_submit';
  return $form;
}

function intercomio_admin_form_submit(&$form, &$form_state) {
  // Remove unchecked values from the settings array
  $custom_data = array_filter($form_state['values']['intercomio_custom_data']);
  variable_set('intercomio_custom_data', $custom_data);
}

function intercomio_mapping_admin_form($form, &$form_state) {
  $fields = field_info_instances('user', NULL);
  $form['description'] = array(
    '#markup' => t('<p>Use this form to map custom user fields over to Intercom.io</p>'),
  );
  $form['intercomio_mapping'] = array(
    '#type' => 'container',
    '#theme' => 'intercomio_mapping_table',
    '#tree' => TRUE,
  );
  $mappings = variable_get('intercomio_mapping', array());
  foreach ($fields['user'] as $id => $field) {
    $form['intercomio_mapping'][$id]['label'] = array(
      '#markup' => $field['label'],
    );
    $form['intercomio_mapping'][$id]['mapping'] = array(
      '#type' => 'textfield',
      '#default_value' => !empty($mappings[$id]['mapping']) ? $mappings[$id]['mapping'] : NULL,
    );
  }
  return system_settings_form($form);
}

function intercomio_events_admin_form($form, &$form_state) {

  $form['intercomio_page_load_events'] = array(
    '#type' => 'textarea',
    '#title' => t('Page load events'),
    '#description' => t('One per line with the syntax: <em>event_name|page_path</em>. Same event name can be used multiple times.'),
    '#rows' => 10,
    '#default_value' => variable_get('intercomio_page_load_events'),
  );

  $form['intercomio_link_click_events'] = array(
    '#type' => 'textarea',
    '#title' => t('Link click events'),
    '#description' => t('One event per line with the syntax: <em>event_name|url</em>. Same event name can be used multiple times.'),
    '#rows' => 10,
    '#default_value' => variable_get('intercomio_link_click_events'),
  );

  return system_settings_form($form);
}