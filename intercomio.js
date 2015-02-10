
(function($) {

  /**
   * Trigger Intercom.io events on click for links with 'data-intercom-event' attribute
   */
  Drupal.behaviors.intercomioDataLinkClickEvents = {
    attach: function(context, settings) {
      $('a[data-intercom-event]', context)
        .once('intercomio-click')
        .each(function(){
          var $this = $(this);
          var event_name = $this.data('intercom-event');
          $this.bind('click', function(){
            if (typeof Intercom != 'undefined') {
              try {
                Intercom('trackEvent', event_name);
              }
              catch(e) {}
            }
          })
        });
    }
  };

  /**
   * Trigger Intercom.io events on click for links with href specified in settings
   */
  Drupal.behaviors.intercomioHrefLinkClickEvents = {
    attach: function(context, settings) {
      if (typeof settings.intercomio != 'undefined' && typeof settings.intercomio.link_events != 'undefined') {
        for(i in settings.intercomio.link_events) {
          if (settings.intercomio.link_events.hasOwnProperty(i)) {
            var event_name = settings.intercomio.link_events[i].event_name;
            var link_href = settings.intercomio.link_events[i].url;
            $('a[href="' + link_href + '"]')
              .once('intercomio-click')
              .each(function(){
                $(this).bind('click', function() {
                  Intercom('trackEvent', event_name);
                })
              });
          }
        }
      }
    }
  }


})(jQuery);
