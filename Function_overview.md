# 🟢 Tổng quan các hàm trong multi_button

## 1. Khởi tạo
`button_init()`
- `memset()`: reset struct
- Gán `read_pin`, `context`: hàm đọc + dữ liệu đi kèm
- Gán `config` (nếu NULL thì dùng `BUTTON_CONFIG_DEFAULT`)
- Đọc trạng thái ban đầu (debounce start)

## 2. Quản lý callback
- `button_attach_event()`: gán callback cho 1 event cụ thể
- `button_detach_event()`: bỏ callback 1 event
- `button_attach_all_events()`: gán cùng 1 callback cho tất cả event
- `button_detach_all_events()`: bỏ hết callback
- `button_trigger_event()`: gọi hàm callback khi event xảy ra

## 3. Vòng xử lý chính
`button_update()`
- Gọi `read_pin(context)`: đọc GPIO qua hàm wrapper
  - Sử dụng `context` để biết port/pin
- Debounce logic: lọc chống dội
- Nếu state thay đổi:
  - **PRESSED** → gán `pressed=true`
    - Lưu `last_press_time`
    - Tăng `click_count` (nếu trong interval)
    - Trigger `PRESSED` event
    - Nếu advanced: trigger `SEQUENCE_START`
  - **RELEASED** → gán `pressed=false`
    - Lưu `last_release_time`
    - Nếu `ultra_long` đang active → trigger `ULTRA_LONG_PRESS_END`
    - Nếu `long` đang active → trigger `LONG_PRESS_END`
    - Ngược lại → trigger `RELEASED`
- Nếu đang **PRESSED**:
  - Kiểm tra thời gian giữ
    - Đạt `long_press_time` → trigger `LONG_PRESS_START`
    - Đạt `ultra_long_time` → trigger `ULTRA_LONG_PRESS_START`
  - Trigger `HOLD` event (`LONG`/`ULTRA_LONG`)
- Nếu **RELEASED** & `click_count > 0`:
  - Nếu timeout `multi_click_interval`:
    - `click_count==1` → trigger `CLICK`
    - `click_count==2` → trigger `DOUBLE_CLICK`
    - `click_count==3` → trigger `TRIPLE_CLICK`
    - `>=multi_click_count` → trigger `SEQUENCE_COMPLETE`
  - Reset `click_count`
- Nếu advanced enabled → xử lý combo/pattern (extension point)

## 4. Quản lý nhiều nút
`button_update_all()`
- Lặp qua mảng `button[]`, gọi `button_update()` cho từng nút

## 5. Hàm tiện ích (Utility)
- `button_get_press_duration()`: trả về thời gian đang giữ
- `button_get_click_count()`: số lần click trong chuỗi
- `button_is_pressed()`: trạng thái hiện tại
- `button_is_long_pressed()`: đang giữ lâu không?
- `button_set_user_data()`: gán dữ liệu tùy ý vào button
- `button_get_user_data()`
- `button_set_custom_callback()`: callback mở rộng riêng
- `button_set_extension()`: pointer mở rộng
- `button_get_extension()`

## 🔵 Sơ đồ khối tổng thể
```
                +-----------------+
                |  button_init()  |
                +-----------------+
                          |
         +----------------+----------------+
         |                                 |
+-------------------+          +-------------------------+
|  button_update()  |<-------->|  button_update_all()    |
+-------------------+          +-------------------------+
         |
   +-----+-----------------------------------+
   |                                         |
Debounce + State Machine                     |
   |                                         |
   +---------------------------+-------------+
                               |
       +-----------------------+--------------------------+
       |                      |                          |
  Pressed events         Released events           Long/Ultra-long
  (PRESSED, SEQ_START)   (RELEASED, CLICK, ...)    (START, HOLD, END)
```

## 🔶 Nhóm phụ trợ
```
+-------------------------+     +-------------------------+
| button_attach_event()   |     | button_trigger_event()  |
| button_detach_event()   |---->| gọi callback[event]     |
| attach_all / detach_all |     | nếu != NULL             |
+-------------------------+     +-------------------------+

+--------------------------+
| Utility & Extensions     |
| - get_press_duration     |
| - get_click_count        |
| - is_pressed             |
| - set/get user_data      |
| - set/get extension      |
+--------------------------+
```

## ⏱ Sequence Diagram: Button Press Flow

```plaintext
Time ↓

[User Action]        [button_update()]       [read_pin()]         [Event Callback]

    |                       |                      |                      |
    |--- Press button ----->|                      |                      |
    |                       |--- call ------------>|                      |
    |                       |   read_pin(context)  |                      |
    |                       |<-- return true ------|                      |
    |                       |                      |                      |
    |                       |--- debounce -------->|                      |
    |                       |--- detect change ----|                      |
    |                       |--- trigger --------->|--- PRESSED ----------> Callback
    |                       |                      |                      |
    |     (holding...)      |--- check duration --->                      |
    |                       |--- after long_press_time ------------------>| LONG_PRESS_START
    |                       |--- periodically --------------------------->| LONG_PRESS_HOLD
    |                       |--- after ultra_long_press_time ------------>| ULTRA_LONG_PRESS_START
    |                       |--- periodically --------------------------->| ULTRA_LONG_PRESS_HOLD
    |                       |                      |                      |
    |--- Release button --->|                      |                      |
    |                       |--- call ------------>|                      |
    |                       |   read_pin(context)  |                      |
    |                       |<-- return false -----|                      |
    |                       |--- debounce -------->|                      |
    |                       |--- detect release -->                       |
    |                       |--- trigger --------->|--- RELEASED ---------> Callback
    |                       |--- if long active --->|--- LONG_PRESS_END --> Callback
    |                       |--- if ultra active -->|--- ULTRA_LONG_PRESS_END
    |                       |                      |                      |
    |     (idle)            |--- after timeout ----|                      |
    |                       |--- trigger --------->|--- CLICK / DOUBLE / TRIPLE
    |                       |                      |                      |
