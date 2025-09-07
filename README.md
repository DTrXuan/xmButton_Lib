# 📘 Hướng Dẫn Sử Dụng Thư Viện Multi-Button

## 🎯 Giới Thiệu
Thư viện Multi-Button là một thư viện xử lý nút nhấn đa năng, hỗ trợ nhiều loại sự kiện và dễ dàng port cho nhiều loại MCU khác nhau.

## 📦 Cài Đặt

### 1. Thêm Files Vào Project
```
project/
├── main.c
├── multi_button.h
├── multi_button.c
└── Makefile
```

### 2. Cấu Hình Platform
Trong file `main.c`, định nghĩa các macro platform:

```c
// Định nghĩa hàm lấy thời gian (milliseconds)
uint32_t platform_get_time_ms(void) {
    // STM32: return HAL_GetTick();
    // ESP32: return xTaskGetTickCount() * portTICK_PERIOD_MS;
    // AVR: return millis(); // Nếu dùng Arduino framework
    return your_timer_function();
}

// Định nghĩa hàm delay
void platform_delay_ms(uint32_t ms) {
    // STM32: HAL_Delay(ms);
    // ESP32: vTaskDelay(ms / portTICK_PERIOD_MS);
    // Non-blocking delay implementation
}

#define BUTTON_GET_TIME() platform_get_time_ms()
#define BUTTON_DELAY_MS(ms) platform_delay_ms(ms)

#include "multi_button.h"
```

## 🚀 Quick Start

### 1. Khởi Tạo Nút Nhấn Cơ Bản
```c
#include "multi_button.h"

// Hàm đọc trạng thái chân
bool read_button_pin(void* context) {
    uint8_t pin = (uint8_t)(uintptr_t)context;
    // return HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_SET; // STM32
    // return gpio_get_level(pin); // ESP32
    return your_read_pin_function(pin);
}

// Callback xử lý sự kiện
void button_event_handler(button_handle_t* button, button_event_t event) {
    int button_id = (int)button->user_data;
    
    switch (event) {
        case BUTTON_EVENT_PRESSED:
            printf("Button %d pressed\n", button_id);
            break;
        case BUTTON_EVENT_CLICK:
            printf("Button %d clicked\n", button_id);
            break;
        // Thêm các event khác...
    }
}

int main() {
    button_handle_t my_button;
    
    // Khởi tạo với config mặc định
    button_init(&my_button, read_button_pin, (void*)1, NULL);
    button_set_user_data(&my_button, (void*)1);
    
    // Gắn callback
    button_attach_event(&my_button, BUTTON_EVENT_CLICK, button_event_handler);
    button_attach_event(&my_button, BUTTON_EVENT_PRESSED, button_event_handler);
    
    while (1) {
        button_update(&my_button);
        BUTTON_DELAY_MS(10);
    }
}
```

## ⚙️ Cấu Hình

### Config Mặc Định
```c
button_config_t config_default = {
    .debounce_time_ms = 25,         // Thời gian chống rung
    .click_time_ms = 400,           // Thời gian xác định click
    .long_press_time_ms = 1000,     // Thời gian long press
    .ultra_long_press_time_ms = 3000, // Thời gian ultra long press
    .multi_click_interval_ms = 300, // Khoảng cách giữa các click
    .sequence_timeout_ms = 2000,    // Timeout cho sequence
    .multi_click_count = 3,         // Số click cho multi-click
    .active_low = true,             // Chế độ active low (pull-up)
    .enable_advanced_events = false // Bật event nâng cao
};
```

### Custom Config
```c
button_config_t custom_config = {
    .debounce_time_ms = 30,
    .click_time_ms = 500,
    .long_press_time_ms = 1500,
    .ultra_long_press_time_ms = 5000,
    .multi_click_interval_ms = 400,
    .multi_click_count = 4,
    .active_low = true,
    .enable_advanced_events = true
};

button_init(&button, read_pin_func, context, &custom_config);
```

## 🎪 Các Loại Sự Kiện

### Basic Events
```c
BUTTON_EVENT_PRESSED          // Nhấn xuống
BUTTON_EVENT_RELEASED         // Nhả ra
BUTTON_EVENT_CLICK            // Click đơn
BUTTON_EVENT_DOUBLE_CLICK     // Double click
BUTTON_EVENT_TRIPLE_CLICK     // Triple click
```

### Long Press Events
```c
BUTTON_EVENT_LONG_PRESS_START   // Bắt đầu long press
BUTTON_EVENT_LONG_PRESS_HOLD    // Giữ long press (gọi liên tục)
BUTTON_EVENT_LONG_PRESS_END     // Kết thúc long press

BUTTON_EVENT_ULTRA_LONG_PRESS_START  // Ultra long press (>3s)
BUTTON_EVENT_ULTRA_LONG_PRESS_HOLD
BUTTON_EVENT_ULTRA_LONG_PRESS_END
```

### Advanced Events
```c
BUTTON_EVENT_SEQUENCE_START      // Bắt đầu chuỗi click
BUTTON_EVENT_SEQUENCE_COMPLETE   // Hoàn thành chuỗi
BUTTON_EVENT_COMBO_START         // Combo nhiều nút
BUTTON_EVENT_COMBO_COMPLETE      // Hoàn thành combo

// Custom events cho mở rộng
BUTTON_EVENT_CUSTOM_1
BUTTON_EVENT_CUSTOM_2
BUTTON_EVENT_CUSTOM_3
```

## 📋 API Reference

### Initialization
```c
void button_init(button_handle_t* button, 
                button_read_pin_t read_func, 
                void* context, 
                const button_config_t* config);
```

### Event Management
```c
// Gắn callback cho từng event
void button_attach_event(button_handle_t* button, 
                        button_event_t event, 
                        button_event_callback_t callback);

// Gỡ callback
void button_detach_event(button_handle_t* button, button_event_t event);

// Gắn callback cho tất cả events
void button_attach_all_events(button_handle_t* button, 
                             button_event_callback_t callback);

// Kích hoạt event thủ công
void button_trigger_event(button_handle_t* button, button_event_t event);
```

### Button Processing
```c
// Cập nhật trạng thái nút
void button_update(button_handle_t* button);

// Cập nhật nhiều nút cùng lúc
void button_update_all(button_handle_t* buttons[], uint8_t count);
```

### Utility Functions
```c
// Lấy thông tin
uint32_t button_get_press_duration(const button_handle_t* button);
uint8_t button_get_click_count(const button_handle_t* button);
bool button_is_pressed(const button_handle_t* button);
bool button_is_long_pressed(const button_handle_t* button);

// Quản lý user data
void button_set_user_data(button_handle_t* button, void* user_data);
void* button_get_user_data(const button_handle_t* button);
```

## 🎯 Ví Dụ Sử Dụng

### 1. Xử Lý Multiple Buttons
```c
button_handle_t btn1, btn2, btn3;

void setup_buttons() {
    button_init(&btn1, read_pin, (void*)1, NULL);
    button_init(&btn2, read_pin, (void*)2, NULL);
    button_init(&btn3, read_pin, (void*)3, NULL);
    
    button_set_user_data(&btn1, (void*)1);
    button_set_user_data(&btn2, (void*)2);
    button_set_user_data(&btn3, (void*)3);
    
    // Gắn callback
    button_attach_event(&btn1, BUTTON_EVENT_CLICK, btn_click_handler);
    button_attach_event(&btn2, BUTTON_EVENT_LONG_PRESS_START, btn_longpress_handler);
}

void main_loop() {
    button_handle_t* buttons[] = {&btn1, &btn2, &btn3};
    button_update_all(buttons, 3);
    BUTTON_DELAY_MS(10);
}
```

### 2. Advanced Event Handling
```c
void advanced_event_handler(button_handle_t* button, button_event_t event) {
    switch (event) {
        case BUTTON_EVENT_CLICK:
            handle_single_click(button);
            break;
        case BUTTON_EVENT_DOUBLE_CLICK:
            handle_double_click(button);
            break;
        case BUTTON_EVENT_LONG_PRESS_START:
            start_long_press_action(button);
            break;
        case BUTTON_EVENT_ULTRA_LONG_PRESS_START:
            emergency_shutdown(button);
            break;
        case BUTTON_EVENT_SEQUENCE_COMPLETE:
            if (button_get_click_count(button) == 5) {
                enable_admin_mode();
            }
            break;
    }
}
```

### 3. Custom Callback với User Data
```c
void custom_callback(button_handle_t* button, void* data) {
    printf("Custom action with data: %s\n", (char*)data);
}

// Setup
button_set_custom_callback(&my_button, custom_callback, "SecretData");
```

## 🔧 Porting Guide

### 1. Định Nghĩa Platform Functions
```c
// STM32 Example
uint32_t platform_get_time_ms(void) {
    return HAL_GetTick();
}

void platform_delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

// ESP32 Example
uint32_t platform_get_time_ms(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void platform_delay_ms(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}
```

### 2. Implement Pin Read Function
```c
// STM32
bool read_pin_stm32(void* context) {
    uint16_t pin = (uint16_t)(uintptr_t)context;
    return HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_SET;
}

// ESP32
bool read_pin_esp32(void* context) {
    int pin = (int)context;
    return gpio_get_level(pin);
}

// Generic MCU
bool read_pin_generic(void* context) {
    volatile uint32_t* port = (volatile uint32_t*)context;
    return (*port & 0x01) != 0;
}
```

## 🐛 Debugging

### Enable Debug Output
```c
void button_event_handler(button_handle_t* button, button_event_t event) {
    const char* event_names[] = {
        "PRESSED", "RELEASED", "CLICK", "DOUBLE_CLICK", "TRIPLE_CLICK",
        "LONG_PRESS_START", "LONG_PRESS_HOLD", "LONG_PRESS_END",
        "ULTRA_LONG_START", "ULTRA_LONG_HOLD", "ULTRA_LONG_END",
        "SEQUENCE_START", "SEQUENCE_COMPLETE", "COMBO_START", "COMBO_COMPLETE",
        "CUSTOM_1", "CUSTOM_2", "CUSTOM_3", "NONE"
    };
    
    printf("Button %d: %s\n", (int)button->user_data, event_names[event]);
    
    // Debug thông tin thêm
    if (event == BUTTON_EVENT_LONG_PRESS_HOLD) {
        printf("Press duration: %lums\n", button_get_press_duration(button));
    }
}
```

## 📝 Best Practices

### 1. Timing Configuration
```c
// Cho button vật lý thông thường
config.debounce_time_ms = 20-50ms
config.click_time_ms = 300-500ms
config.long_press_time_ms = 800-1200ms

// Cho touch button
config.debounce_time_ms = 50-100ms
config.click_time_ms = 200-400ms
```

### 2. Memory Management
```c
// Sử dụng static allocation cho embedded systems
static button_handle_t buttons[MAX_BUTTONS];

// Hoặc dynamic allocation nếu MCU hỗ trợ
button_handle_t* buttons = malloc(sizeof(button_handle_t) * num_buttons);
```

### 3. Performance Optimization
```c
// Sử dụng button_update_all() cho nhiều nút
button_handle_t* button_list[] = {&btn1, &btn2, &btn3, &btn4};
button_update_all(button_list, 4);

// Tránh blocking delay trong callbacks
void button_handler(button_handle_t* button, button_event_t event) {
    // Tốt: Set flags và xử lý sau
    if (event == BUTTON_EVENT_CLICK) {
        button_click_flag = true;
    }
    
    // Không tốt: Blocking operations
    // HAL_Delay(1000); // ❌ Tránh làm điều này
}
```

## 🎨 Extension Examples

### Combo Detection Extension
```c
#include "multi_button.h"

typedef struct {
    button_handle_t** buttons;
    uint8_t count;
    uint32_t timeout;
} button_combo_t;

void check_combo(button_combo_t* combo) {
    uint8_t pressed = 0;
    for (uint8_t i = 0; i < combo->count; i++) {
        if (button_is_pressed(combo->buttons[i])) {
            pressed++;
        }
    }
    
    if (pressed == combo->count) {
        printf("Combo activated!\n");
    }
}
```

## 📊 Event Flow Diagram
```
Raw Input → Debounce → State Machine → Event Detection → Callback Execution
    │          │            │               │                  │
    │          │            ├→ Pressed ─────┼→ BUTTON_EVENT_PRESSED
    │          │            ├→ Released ────┼→ BUTTON_EVENT_RELEASED  
    │          │            ├→ Click ───────┼→ BUTTON_EVENT_CLICK
    │          │            ├→ Long Press ──┼→ BUTTON_EVENT_LONG_PRESS_*
    │          │            └→ Multi-Click ─┼→ BUTTON_EVENT_*_CLICK
    │          │                            └→ BUTTON_EVENT_SEQUENCE_*
```