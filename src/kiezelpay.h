/*
* KiezelPay Integration Library - v1.4 - Copyright Kiezel 2016
*
* BECAUSE THE LIBRARY IS LICENSED FREE OF CHARGE, THERE IS NO
* WARRANTY FOR THE LIBRARY, TO THE EXTENT PERMITTED BY APPLICABLE
* LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
* HOLDERS AND/OR OTHER PARTIES PROVIDE THE LIBRARY "AS IS"
* WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE
* RISK AS TO THE QUALITY AND PERFORMANCE OF THE LIBRARY IS WITH YOU.
* SHOULD THE LIBRARY PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
* NECESSARY SERVICING, REPAIR OR CORRECTION.
*
* IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN
* WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY
* MODIFY AND/OR REDISTRIBUTE THE LIBRARY AS PERMITTED ABOVE, BE
* LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,
* INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR
* INABILITY TO USE THE LIBRARY (INCLUDING BUT NOT LIMITED TO LOSS
* OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
* YOU OR THIRD PARTIES OR A FAILURE OF THE LIBRARY TO OPERATE WITH
* ANY OTHER SOFTWARE), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#pragma once
#include <pebble.h>
/**
  Set to 1 to enable verbose logging, handy for tracking down issues with the kiezelpay integration

  Set to 0 before releasing
*/
#define KIEZELPAY_LOG_VERBOSE 0

/**
  When set to 1, you can test the purchasing of your app without having to pay for real.
  When set to 1, trial times will always be 30 seconds (only when time trial is enabled here and on the server)
  When set to 1, periodic status re-checks will happen every 60 seconds (only when enabled)

  Test purchases only, set to 0 before releasing or users can get your app for free!
*/
#define KIEZELPAY_TEST_MODE 0

/**
  Set to 1 to remove all code which has to do with the time based trial. This saves memory and code space in case you don't use it.
  You need to manually signal that the trial is over and purchase needs to start by calling kiezelpay_start_purchase();

  Set to 0 to let kiezelpay automatically initiate the purchase as soon as the time-based trial configured on the server has ended.
  In case no time based trial is configured on the server, this will require the user to purchase the app immediately after installing it.
*/
#define KIEZELPAY_DISABLE_TIME_TRIAL 0

/**
  Set to 1 when you want to remove all code that has to do with message display to save memory and code space, in this mode the message display needs to be handled by yourself!

  Set to 0 to let the kiezelpay lib show messages itself in case you don't handle them (e.g. purchasing, messages about internet connection issues, ...)
*/
#define KIEZELPAY_DISABLE_MESSAGES 0

/**
  Set to 1 to disable period re-checks of the licensed status on the server. This saves memory and code space.

  RECOMMENDED: Set to 0 to let kiezelpay automatically recheck the licensed state of this app online after it was purchased,
  this prevents users from tampering with the stored license data (e.g. copy it from another licensed device) and makes it more secure.
  The duration between these checks is managed by the KiezelPay server, but will always be 24 hours or longer.
  This check will not lock down the app in case there is no internet available or the server cannot be reached for any other reason,
  only when the kiezelpay effectively returns the status "unlicensed" will the user be shown the purchase dialog
*/
#define KIEZELPAY_DISABLE_PERIODIC_CHECKS 0


/**
  Default messages shown to the user in different stages of the purchase or when errors occur.
  Only used when the default KiezelPay messages are enabled (#define KIEZELPAY_DISABLE_MESSAGES 0)

  You can change the messages to your liking here. Make sure to check if the changed message still fits the display of all different pebble watches.
*/
#define KIEZELPAY_UNKNOWN_ERROR_MSG "An unknown error occurred"
#define KIEZELPAY_BLUETOOTH_UNAVAILABLE_MSG "There is a problem with the connection between your watch and your phone"
#define KIEZELPAY_INTERNET_UNAVAILABLE_MSG "There is a problem with the internet connection of your phone"
/**
  You can replace "code" with anything you want in the url, it will all work, e.g.: "kzl.io/myappname".
  It is also possible to configure that custom url in your product settings on our website so its shows your personalized purchase page to the customers
*/
#define KIEZELPAY_CODE_AVAILABLE_MSG "To continue using this product please visit kzl.io/code and enter this code:"
#define KIEZELPAY_PURCHASE_STARTED_MSG "Please complete the purchase process to unlock this app"
#define KIEZELPAY_LICENSED_MSG "Thank you for your purchase!"

/**
  All events that can be fired by kiezelpay lib.
  The event types are flags which can be combined in the output of the function kiezelpay_get_status()
*/
typedef enum {
  KIEZELPAY_NO_EVENT              = 0,
  KIEZELPAY_BLUETOOTH_UNAVAILABLE = 1 << 0,       /**< Cannot send appmessage to phone */
  KIEZELPAY_INTERNET_UNAVAILABLE  = 1 << 1,       /**< Cannot connect with kiezelpay server */
  KIEZELPAY_ERROR                 = 1 << 2,       /**< Generic error (e.g. invalid message format) */
#if KIEZELPAY_DISABLE_TIME_TRIAL == 0
  KIEZELPAY_TRIAL_STARTED         = 1 << 3,       /**< Trial has started (trial end time [uint32_t] as extra data) ==> unlock all features available during trial */
#endif
  KIEZELPAY_TRIAL_ENDED           = 1 << 4,       /**< Trial has ended, kiezelpay lib is attempting to initiate the purchase on the server ==> disable all trial features */
  KIEZELPAY_CODE_AVAILABLE        = 1 << 5,       /**< A purchase transaction has been initiated on the kiezelpay server (purchase code [uint32_t] as extra data) */
  KIEZELPAY_PURCHASE_STARTED      = 1 << 6,       /**< The user entered the code on the kiezelpay website and purchase is in progress (purchase code [uint32_t] as extra data) */
  KIEZELPAY_LICENSED              = 1 << 7        /**< User has successfully completed the purchase and is now licensed to use your app ==> unlock all features available after purchase */
} kiezelpay_event;

/**
  Handler signature to catch kiezelpay events.
  Return true if you handled the event, return false to let the KiezelPay library handle events and show the correct messages to the user
*/
typedef bool (*kiezelpay_event_handler)(kiezelpay_event,void*);

/** Handler signature to get notified of inbox received AppMessage events */
typedef void (*inbox_received_handler)(DictionaryIterator*,void*);
/** Handler signature to get notified of inbox dropped AppMessage events */
typedef void (*inbox_dropped_handler)(AppMessageResult,void*);
/** Handler signature to get notified of outbox failed AppMessage events */
typedef void (*outbox_failed_handler)(DictionaryIterator*,AppMessageResult,void*);
/** Handler signature to get notified of outbox sent AppMessage events */
typedef void (*outbox_sent_handler)(DictionaryIterator*,void*);


/**
  Kiezelpay_config struct used to pass along several settings to the kiezelpay lib.
  Use the kiezelpay_settings instance for your configuration changes.
*/
typedef struct {
  /**
    Override AppMessage inbox and outbox size
  */
  uint16_t messaging_inbox_size;
  uint16_t messaging_outbox_size;

  /**
    Kiezelpay Event; assign a handler to receive interesting kiezelpay events. You can then choose to act upon them yourself, or let kiezelpay handle it.
  */
  kiezelpay_event_handler on_kiezelpay_event;

  /**
    AppMessage events; assign to receive incoming AppMessages in your app
  */
  inbox_received_handler on_inbox_received;
  inbox_dropped_handler on_inbox_dropped;
  outbox_failed_handler on_outbox_failed;
  outbox_sent_handler on_outbox_sent;
} kiezelpay_config;

/**
  KiezelPay settings instance, modify to override default settings.
*/
extern kiezelpay_config kiezelpay_settings;

/** Call once on watchface/app init before using any other functions */
void kiezelpay_init();

/** Call on watchface/app deinit to clean up mess */
void kiezelpay_deinit();

/**
  Signal the kiezelpay lib that the user needs to purchase to continue. You need to call this when
  there is no trial (KIEZELPAY_DISABLE_TIME_TRIAL == 1), you can call also it when you do have a trial
  in case you want to start the purchase process before the trial ends. If you dont call it in that case
  the purchase will start automatically as soon as the trial ends
*/
void kiezelpay_start_purchase();

/**
  Signal the kiezelpay lib that a purchase is no longer necesarry (e.g. the user backed out of the premium feature and does not want to use it anyway).
*/
void kiezelpay_cancel_purchase();

#if KIEZELPAY_DISABLE_TIME_TRIAL == 0
/**
  Get the time when the current trial will end. You can use this to inform the user how long his trial still lasts.
  This trial end time is also given along with the KIEZELPAY_TRIAL_STARTED event
*/
time_t kiezelpay_get_trial_end_time();
#endif

/**
  Get user status info ==> this info is also communicated with the on_kiezelpay_event handler, you can use these to get the status at any other time as well.
  The return value of this function can be any combination of the kiezelpay_event values!
  e.g. trial ended and no internet; result == KIEZELPAY_TRIAL_ENDED | KIEZELPAY_NO_INTERNET
   ==> test the values like "if (result & KIEZELPAY_TRIAL_ENDED) { }"
  BEWARE: this function can return the wrong result in the situation that a user deleted the app AFTER having purchased it, then later reinstalls the app.
  In that case the library "thinks" this is a clean install and will NOT return the status "KIEZELPAY_LICENSED" until there was communication with the
  kiezelpay server and it received the licensed status from the kiezelpay server (should not be more then a few seconds after installation).
  Keep this in mind when showing messages to the user based on the result of this function.
*/
int32_t kiezelpay_get_status();
