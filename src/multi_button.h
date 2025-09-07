#ifndef MULTI_BUTTON_H
#define MULTI_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Platform abstraction - user must define these
#ifndef BUTTON_GET_TIME
#define BUTTON_GET_TIME() 0
#endif

#ifndef BUTTON_DELAY_MS
#define BUTTON_DELAY_MS(ms)
#endif

// Button events
typedef enum {
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED,
    BUTTON_EVENT_CLICK,
    BUTTON_EVENT_DOUBLE_CLICK,
    BUTTON_EVENT_TRIPLE_CLICK,
    BUTTON_EVENT_LONG_PRESS_START,
    BUTTON_EVENT_LONG_PRESS_HOLD,
    BUTTON_EVENT_LONG_PRESS_END,
    BUTTON_EVENT_ULTRA_LONG_PRESS_START,
    BUTTON_EVENT_ULTRA_LONG_PRESS_HOLD,
    BUTTON_EVENT_ULTRA_LONG_PRESS_END,
    BUTTON_EVENT_SEQUENCE_START,
    BUTTON_EVENT_SEQUENCE_COMPLETE,
    BUTTON_EVENT_COMBO_START,
    BUTTON_EVENT_COMBO_COMPLETE,
    BUTTON_EVENT_CUSTOM_1,
    BUTTON_EVENT_CUSTOM_2,
    BUTTON_EVENT_CUSTOM_3,
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_COUNT // Total number of events
} button_event_t;

// Button configuration
typedef struct {
    uint32_t debounce_time_ms;
    uint32_t click_time_ms;
    uint32_t long_press_time_ms;
    uint32_t ultra_long_press_time_ms;
    uint32_t multi_click_interval_ms;
    uint32_t sequence_timeout_ms;
    uint8_t multi_click_count;
    bool active_low;
    bool enable_advanced_events;
} button_config_t;

// Button handle
typedef struct button_handle_t button_handle_t;

// Callback function types
typedef void (*button_event_callback_t)(button_handle_t* button, button_event_t event);
typedef void (*button_custom_callback_t)(button_handle_t* button, void* data);

// Pin read function type
typedef bool (*button_read_pin_t)(void* context);

// Button handle structure
struct button_handle_t {
    // Hardware interface
    button_read_pin_t read_pin;
    void* context;

    // Configuration
    button_config_t config;

    // State machine
    uint32_t last_change_time;
    uint32_t last_press_time;
    uint32_t last_release_time;
    uint32_t click_count;
    uint32_t sequence_counter;
    bool last_raw_state;
    bool debounced_state;
    bool pressed;
    bool long_pressed;
    bool ultra_long_pressed;
    bool in_sequence;

    // Individual event callbacks
    button_event_callback_t event_callbacks[BUTTON_EVENT_COUNT];

    // Custom data and callbacks
    void* user_data;
    button_custom_callback_t custom_callback;
    void* custom_data;

    // Extension pointer for future features
    void* extension;
};

// Default configuration
#define BUTTON_CONFIG_DEFAULT { \
    .debounce_time_ms = 25, \
    .click_time_ms = 400, \
    .long_press_time_ms = 1000, \
    .ultra_long_press_time_ms = 3000, \
    .multi_click_interval_ms = 300, \
    .sequence_timeout_ms = 2000, \
    .multi_click_count = 3, \
    .active_low = true, \
    .enable_advanced_events = false \
}

// Function prototypes
void button_init(button_handle_t* button,
                button_read_pin_t read_func,
                void* context,
                const button_config_t* config);

// Event callback management
void button_attach_event(button_handle_t* button, button_event_t event, button_event_callback_t callback);
void button_detach_event(button_handle_t* button, button_event_t event);
void button_attach_all_events(button_handle_t* button, button_event_callback_t callback);
void button_detach_all_events(button_handle_t* button);

// Button processing
void button_update(button_handle_t* button);
void button_update_all(button_handle_t* buttons[], uint8_t count);

// Utility functions
uint32_t button_get_press_duration(const button_handle_t* button);
uint8_t button_get_click_count(const button_handle_t* button);
bool button_is_pressed(const button_handle_t* button);
bool button_is_long_pressed(const button_handle_t* button);

// User data management
void button_set_user_data(button_handle_t* button, void* user_data);
void* button_get_user_data(const button_handle_t* button);

// Custom callback setup
void button_set_custom_callback(button_handle_t* button, button_custom_callback_t callback, void* data);

// Extension system
void button_set_extension(button_handle_t* button, void* extension);
void* button_get_extension(const button_handle_t* button);

// Helper functions for common operations
void button_trigger_event(button_handle_t* button, button_event_t event);

#ifdef __cplusplus
}
#endif

#endif // MULTI_BUTTON_H
