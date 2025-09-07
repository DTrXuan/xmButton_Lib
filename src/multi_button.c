#include "multi_button.h"
#include <string.h>

void button_init(button_handle_t* button,
                button_read_pin_t read_func,
                void* context,
                const button_config_t* config) {

    if (!button || !read_func) return;

    memset(button, 0, sizeof(button_handle_t));

    button->read_pin = read_func;
    button->context = context;

    // Set configuration
    if (config) {
        button->config = *config;
    } else {
        button->config = (button_config_t)BUTTON_CONFIG_DEFAULT;
    }

    // Read initial state
    bool raw_state = button->read_pin(button->context);
    if (button->config.active_low) {
        raw_state = !raw_state;
    }

    button->last_raw_state = raw_state;
    button->debounced_state = raw_state;
    button->last_change_time = BUTTON_GET_TIME();
}

// Event callback management
void button_attach_event(button_handle_t* button, button_event_t event, button_event_callback_t callback) {
    if (!button || event >= BUTTON_EVENT_COUNT) return;
    button->event_callbacks[event] = callback;
}

void button_detach_event(button_handle_t* button, button_event_t event) {
    if (!button || event >= BUTTON_EVENT_COUNT) return;
    button->event_callbacks[event] = NULL;
}

void button_attach_all_events(button_handle_t* button, button_event_callback_t callback) {
    if (!button) return;
    for (button_event_t event = 0; event < BUTTON_EVENT_COUNT; event++) {
        button->event_callbacks[event] = callback;
    }
}

void button_detach_all_events(button_handle_t* button) {
    if (!button) return;
    for (button_event_t event = 0; event < BUTTON_EVENT_COUNT; event++) {
        button->event_callbacks[event] = NULL;
    }
}

void button_trigger_event(button_handle_t* button, button_event_t event) {
    if (!button || event >= BUTTON_EVENT_COUNT) return;

    if (button->event_callbacks[event]) {
        button->event_callbacks[event](button, event);
    }
}

void button_update(button_handle_t* button) {
    if (!button || !button->read_pin) return;

    uint32_t current_time = BUTTON_GET_TIME();
    bool raw_state = button->read_pin(button->context);

    // Apply active low logic
    if (button->config.active_low) {
        raw_state = !raw_state;
    }

    // Debouncing logic
    if (raw_state != button->last_raw_state) {
        button->last_change_time = current_time;
        button->last_raw_state = raw_state;
    }

    uint32_t time_since_change = current_time - button->last_change_time;

    if (time_since_change >= button->config.debounce_time_ms) {
        if (raw_state != button->debounced_state) {
            button->debounced_state = raw_state;

            if (button->debounced_state) { // Button pressed
                button->pressed = true;
                button->last_press_time = current_time;
                button->long_pressed = false;
                button->ultra_long_pressed = false;

                // Check for multi-click sequence
                if ((current_time - button->last_release_time) < button->config.multi_click_interval_ms) {
                    button->click_count++;
                } else {
                    button->click_count = 1;
                }

                // Trigger pressed event
                button_trigger_event(button, BUTTON_EVENT_PRESSED);

                // Check for sequence start
                if (button->config.enable_advanced_events && button->click_count == 1) {
                    button->in_sequence = true;
                    button->sequence_counter = current_time;
                    button_trigger_event(button, BUTTON_EVENT_SEQUENCE_START);
                }

            } else { // Button released
                button->pressed = false;
                button->last_release_time = current_time;
                uint32_t press_duration = current_time - button->last_press_time;

                if (button->ultra_long_pressed) {
                    button->ultra_long_pressed = false;
                    button_trigger_event(button, BUTTON_EVENT_ULTRA_LONG_PRESS_END);
                }
                else if (button->long_pressed) {
                    button->long_pressed = false;
                    button_trigger_event(button, BUTTON_EVENT_LONG_PRESS_END);
                }
                else if (press_duration >= button->config.debounce_time_ms) {
                    button_trigger_event(button, BUTTON_EVENT_RELEASED);
                }
            }
        }
    }

    // Check for long press events
    if (button->pressed) {
        uint32_t press_duration = current_time - button->last_press_time;

        // Ultra long press
        if (!button->ultra_long_pressed && press_duration >= button->config.ultra_long_press_time_ms) {
            button->ultra_long_pressed = true;
            button_trigger_event(button, BUTTON_EVENT_ULTRA_LONG_PRESS_START);
        }
        // Long press
        else if (!button->long_pressed && press_duration >= button->config.long_press_time_ms) {
            button->long_pressed = true;
            button_trigger_event(button, BUTTON_EVENT_LONG_PRESS_START);
        }

        // Hold events
        if (button->ultra_long_pressed) {
            button_trigger_event(button, BUTTON_EVENT_ULTRA_LONG_PRESS_HOLD);
        }
        else if (button->long_pressed) {
            button_trigger_event(button, BUTTON_EVENT_LONG_PRESS_HOLD);
        }
    }

    // Check for click events after release timeout
    if (!button->pressed && button->click_count > 0) {
        uint32_t time_since_release = current_time - button->last_release_time;

        if (time_since_release >= button->config.multi_click_interval_ms) {
            // Trigger appropriate click event
            switch (button->click_count) {
                case 1:
                    button_trigger_event(button, BUTTON_EVENT_CLICK);
                    break;
                case 2:
                    button_trigger_event(button, BUTTON_EVENT_DOUBLE_CLICK);
                    break;
                case 3:
                    button_trigger_event(button, BUTTON_EVENT_TRIPLE_CLICK);
                    break;
                default:
                    if (button->click_count >= button->config.multi_click_count) {
                        button_trigger_event(button, BUTTON_EVENT_SEQUENCE_COMPLETE);
                    }
                    break;
            }

            // Reset sequence if timeout
            if (button->in_sequence &&
                (current_time - button->sequence_counter) > button->config.sequence_timeout_ms) {
                button->in_sequence = false;
            }

            button->click_count = 0;
        }
    }

    // Check for custom events or combinations
    if (button->config.enable_advanced_events) {
        // Here you can add logic for combo detection, pattern recognition, etc.
        // This can be extended through the extension system
    }
}

void button_update_all(button_handle_t* buttons[], uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        if (buttons[i]) {
            button_update(buttons[i]);
        }
    }
}

// Utility functions
uint32_t button_get_press_duration(const button_handle_t* button) {
    if (!button || !button->pressed) return 0;
    return BUTTON_GET_TIME() - button->last_press_time;
}

uint8_t button_get_click_count(const button_handle_t* button) {
    return button ? button->click_count : 0;
}

bool button_is_pressed(const button_handle_t* button) {
    return button ? button->pressed : false;
}

bool button_is_long_pressed(const button_handle_t* button) {
    return button ? button->long_pressed : false;
}

void button_set_user_data(button_handle_t* button, void* user_data) {
    if (button) {
        button->user_data = user_data;
    }
}

void* button_get_user_data(const button_handle_t* button) {
    return button ? button->user_data : NULL;
}

void button_set_custom_callback(button_handle_t* button, button_custom_callback_t callback, void* data) {
    if (button) {
        button->custom_callback = callback;
        button->custom_data = data;
    }
}

void button_set_extension(button_handle_t* button, void* extension) {
    if (button) {
        button->extension = extension;
    }
}

void* button_get_extension(const button_handle_t* button) {
    return button ? button->extension : NULL;
}
