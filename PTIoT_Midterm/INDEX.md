# Kiến thức

# Delay
1000ms = 1 giây

## Led
Chân thẳng đi ra GND
Chân công từ resistor (điện trở) đi vào (chân nâu) đi ra (chân vàng)

Chân tín hiệu: 4, 5, 18, 19

## LCD
- SDA (Chân 21)
- SCL (Chân 22)

# MQTT
MQTTX nó vẫn sử dụng được port của web

## Tạo mã ngẫu nhiên
- [https://www.guidgen.com/](https://www.guidgen.com/)

# Danh sách bài tập theo tuần
- Tuần 5: 
    - Đèn giao thông. [https://wokwi.com/projects/414786022723445761](https://wokwi.com/projects/414786022723445761)

# Danh sách Bài tập
## Bài tập LED
Bài tập 1: Làm ESP32 sáng đèn chân số 5
Bài tập 2: Làm ESP32, nhấp nháy 1s đèn chân số 5

## Bài tập FreeRTOS:
- Bài tập FreeRTOS 1 (baitap_freertos_bai_1): 
    - Nhấp nháy đèn 18, 19 bằng RTOS 
    - [https://wokwi.com/projects/416666374253425665](https://wokwi.com/projects/416666374253425665)
    - [https://wokwi.com/projects/412245447276359681](https://wokwi.com/projects/412245447276359681)

- Bài tập FreeRTOS 2 (baitap_freertos_bai_2):
    - Đấu nối thêm nút nhấn D vào bo mạch, khi nhấn nút D thực hiện bật/tắt LED 1 và LED 2 xen kẽ nhau (LED 1 bật thì LED 2 tắt và ngược lại)?

### Thực hiện sử dụng thư viện FreeRTOS với Queue để thực hiện các yêu cầu sau:
- Bài tập FreeRTOS Queue 1 (baitap_freertos_queue_bai_1):
    - Context: Tạo Task1 với nhiệm vụ đọc dữ liệu nhiệt độ từ cảm biến DHT11 và đưa dữ liệu vào Queue?

- Bài tập FreeRTOS Queue 2 (baitap_freertos_queue_bai_2):
    - Context: Tạo Task2 với nhiệm vụ hiển thị dữ liệu lên màn hình LCD từ dữ liệu lấy được từ Queue?

- Bài tập FreeRTOS Queue 3 (baitap_free_rtos_queue_bai_3):
    - Context: Tạo Task3 đọc dữ liệu từ cảm biến ánh sáng thấp hơn mức ánh sáng môi trường tự nhiên (< 1000 Lux) thì bật đèn?


### Sử dụng Semaphore để đồng bộ hóa các tác vụ với thư viện FreeRTOS thực hiện các yêu cầu sau: 
- Bài tập FreeRTOS Semaphore 1 (baitap_freertos_semaphore_bai_1):
    - Lập trình điều khiển 3 đèn LED với 3 hiệu ứng nhấp nháy khác nhau. Sử dụng FreeRTOS với Semaphore để đồng bộ hóa giữa các tác vụ. 
    - Wokwi: [https://wokwi.com/projects/412254994128655361](https://wokwi.com/projects/412254994128655361)

- Bài tập FreeRTOS Semaphore 2 (baitap_freertos_semaphore_bai_2):
    - Tạo cơ chế để ưu tiên tác vụ cao nhất có quyền điều khiển LED trước, ưu tiên ngay cả khi các tác vụ khác đang thực thi.
    - Wokwi: [https://wokwi.com/projects/412254994128655361](https://wokwi.com/projects/412254994128655361)

- Bài tập FreeRTOS Semaphore 3 (baitap_freertos_semaphore_bai_3):
    - Context: Thêm 2 nút nhấn để điều khiển việc bật hoặc tắt các tác vụ nhấp nháy LED. Khi nhấn nút 1, các tác vụ liên quan đến LED 1 và LED 2 sẽ bị tạm dừng, chỉ còn tác vụ của LED 3 hoạt động. Nhấn nút 2 sẽ kích hoạt lại tất cả các tác vụ.
    - Wokwi: [https://wokwi.com/projects/412254994128655361](https://wokwi.com/projects/412254994128655361)

## Bài tập MQTT