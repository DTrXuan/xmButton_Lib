#include <stdio.h>
#include "multi_button.h"

// Platform functions
uint32_t platform_get_time_ms(void) {
    // Implement based on your MCU
    static uint32_t counter = 0;
    return counter++;
}

#define BUTTON_GET_TIME() platform_get_time_ms()

// Individual event handlers
void on_pressed(button_handle_t* button, button_event_t event) {
    printf("Button %d pressed\n", (int)button->user_data);
}

void on_released(button_handle_t* button, button_event_t event) {
    printf("Button %d released\n", (int)button->user_data);
}

void on_click(button_handle_t* button, button_event_t event) {
    printf("Button %d: Single click\n", (int)button->user_data);
}

void on_double_click(button_handle_t* button, button_event_t event) {
    printf("Button %d: Double click!\n", (int)button->user_data);
}

void on_long_press_start(button_handle_t* button, button_event_t event) {
    printf("Button %d: Long press started\n", (int)button->user_data);
}

void on_long_press_hold(button_handle_t* button, button_event_t event) {
    printf("Button %d: Long press holding (%lums)\n", 
           (int)button->user_data, button_get_press_duration(button));
}

void on_ultra_long_press(button_handle_t* button, button_event_t event) {
    printf("Button %d: Ultra long press!\n", (int)button->user_data);
}

void on_triple_click(button_handle_t* button, button_event_t event) {
    printf("Button %d: Triple click! Special action!\n", (int)button->user_data);
}

void on_sequence_complete(button_handle_t* button, button_event_t event) {
    printf("Button %d: Sequence complete (%d clicks)\n", 
           (int)button->user_data, button_get_click_count(button));
}

// Custom callback example
void custom_action(button_handle_t* button, void* data) {
    printf("Custom action triggered for button %d with data: %s\n", 
           (int)button->user_data, (char*)data);
}

// Pin read function
bool read_button_pin(void* context) {
    // Implement based on your hardware
    // uint8_t pin = (uint8_t)(uintptr_t)context;
    // return HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_SET;
    return false; // Placeholder
}

int main() {
    printf("Advanced Button Library with Individual Callbacks\n");
    
    button_handle_t button1;
    button_config_t config = BUTTON_CONFIG_DEFAULT;
    config.enable_advanced_events = true;
    config.ultra_long_press_time_ms = 5000;
    
    // Initialize button
    button_init(&button1, read_button_pin, (void*)1, &config);
    button_set_user_data(&button1, (void*)1);
    
    // Attach individual callbacks for specific events
    button_attach_event(&button1, BUTTON_EVENT_PRESSED, on_pressed);
    button_attach_event(&button1, BUTTON_EVENT_RELEASED, on_released);
    button_attach_event(&button1, BUTTON_EVENT_CLICK, on_click);
    button_attach_event(&button1, BUTTON_EVENT_DOUBLE_CLICK, on_double_click);
    button_attach_event(&button1, BUTTON_EVENT_TRIPLE_CLICK, on_triple_click);
    button_attach_event(&button1, BUTTON_EVENT_LONG_PRESS_START, on_long_press_start);
    button_attach_event(&button1, BUTTON_EVENT_LONG_PRESS_HOLD, on_long_press_hold);
    button_attach_event(&button1, BUTTON_EVENT_ULTRA_LONG_PRESS_START, on_ultra_long_press);
    button_attach_event(&button1, BUTTON_EVENT_SEQUENCE_COMPLETE, on_sequence_complete);
    
    // Set custom callback
    button_set_custom_callback(&button1, custom_action, "Secret data");
    
    // Main loop
    while (1) {
        button_update(&button1);
        
        // Manually trigger custom event if needed
        if (button_get_click_count(&button1) == 5) {
            button_trigger_event(&button1, BUTTON_EVENT_CUSTOM_1);
        }
        
        BUTTON_DELAY_MS(10);
    }
    
    return 0;
}