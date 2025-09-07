Multi-Button Library
Thư viện xử lý nút nhấn đa năng cho MCU, hỗ trợ nhiều loại sự kiện và dễ dàng porting cho nhiều nền tảng.

📦 Tính Năng
✅ Hỗ trợ đa nút nhấn cùng lúc

✅ Xử lý debounce chống rung

✅ Callback riêng cho từng sự kiện

✅ Hỗ trợ nhiều loại sự kiện: click, double click, long press,...

✅ Dễ dàng mở rộng và tùy chỉnh

✅ Portable cho nhiều loại MCU

✅ Configurable timing parameters

🚀 Quick Start
1. Thêm files vào project
bash
project/
├── main.c
├── multi_button.h
├── multi_button.c
2. Cấu hình platform
c
// Định nghĩa trong main.c
uint32_t platform_get_time_ms(void) {
    return HAL_GetTick();  // STM32
    // return xTaskGetTickCount() * portTICK_PERIOD_MS;  // ESP32
}

#define BUTTON_GET_TIME() platform_get_time_ms()
#define BUTTON_DELAY_MS(ms) your_delay_function(ms)
3. Khởi tạo và sử dụng
c
#include "multi_button.h"

bool read_pin_func(void* context) {
    uint8_t pin = (uint8_t)context;
    return HAL_GPIO_ReadPin(GPIOA, pin);
}

void event_handler(button_handle_t* button, button_event_t event) {
    switch(event) {
        case BUTTON_EVENT_CLICK:
            printf("Single click\n");
            break;
        case BUTTON_EVENT_DOUBLE_CLICK:
            printf("Double click\n");
            break;
    }
}

int main() {
    button_handle_t my_button;
    button_init(&my_button, read_pin_func, (void*)1, NULL);
    button_attach_event(&my_button, BUTTON_EVENT_CLICK, event_handler);
    
    while(1) {
        button_update(&my_button);
        BUTTON_DELAY_MS(10);
    }
}
📋 API Reference
Khởi tạo
c
void button_init(button_handle_t* button, 
                button_read_pin_t read_func, 
                void* context, 
                const button_config_t* config);
Quản lý sự kiện
c
void button_attach_event(button_handle_t* button, 
                        button_event_t event, 
                        button_event_callback_t callback);
void button_detach_event(button_handle_t* button, button_event_t event);
Xử lý
c
void button_update(button_handle_t* button);
void button_update_all(button_handle_t* buttons[], uint8_t count);
🎯 Các Sự Kiện Hỗ Trợ
Sự Kiện	Mô Tả
BUTTON_EVENT_PRESSED	Nhấn nút
BUTTON_EVENT_RELEASED	Thả nút
BUTTON_EVENT_CLICK	Click đơn
BUTTON_EVENT_DOUBLE_CLICK	Double click
BUTTON_EVENT_LONG_PRESS_START	Bắt đầu long press
BUTTON_EVENT_LONG_PRESS_HOLD	Giữ long press
BUTTON_EVENT_LONG_PRESS_END	Kết thúc long press
🔧 Porting Guide
STM32
c
uint32_t platform_get_time_ms(void) {
    return HAL_GetTick();
}

bool read_pin_stm32(void* context) {
    uint16_t pin = (uint16_t)context;
    return HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_SET;
}
ESP32
c
uint32_t platform_get_time_ms(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

bool read_pin_esp32(void* context) {
    int pin = (int)context;
    return gpio_get_level(pin);
}
📝 Examples
Xem thêm các ví dụ trong thư mục examples:

Single Button

Multiple Buttons

Advanced Events

📄 License
MIT License - Xem file LICENSE để biết thêm chi tiết.