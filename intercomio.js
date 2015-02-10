
(function($) {

  /**
   * Trigger Intercom.io events on click for links with 'data-intercom-event' attribute
   */
  Drupal.behaviors.intercomioLinkClickEvents = {
    attach: function(context, settings) {
      $('a[data-intercom-event]').each(function(){
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

})(jQuery);
