# Giai đoạn 1: Điều khiển Relay & Chốt cửa

## 🎯 Mục tiêu
Mục tiêu chính của giai đoạn này là thiết lập khả năng điều khiển phần cứng cơ bản để thực hiện việc đóng/mở khóa:
- Kết nối thành công ESP32 với Module Relay 5V.
- Lập trình logic điều khiển để kích hoạt relay (trigger).
- Kiểm tra tính ổn định của cơ chế đóng/mở cơ học trước khi tích hợp các tính năng thông minh (Face ID).

## 🛠️ Phần cứng hiện có (Hardware)
Các linh kiện tham gia trực tiếp vào giai đoạn này:
- **ESP32 DevKit V1**: Bo mạch điều khiển chính.
- **Module Relay 5V**: Đóng vai trò công tắc điện tử để điều khiển chốt cửa.
- **Chốt cửa (Solenoid Lock)**: Thiết bị thực hiện việc khóa/mở cửa.
- **Nguồn tổ ong 12V**: Cung cấp điện cho chốt cửa và ESP32.

## 🚀 Cách đạt được mục tiêu
Quy trình thực hiện được chia thành các bước cụ thể sau:

### 1. Kết nối mạch điện (Wiring)
*   Sử dụng nguồn 12V từ bộ nguồn tổ ong cho chốt cửa và hạ áp xuống 5V cho ESP32/Relay.
*   Kết nối chân **IN** của Relay với chân **GPIO 32** trên ESP32.  

### 2. Lập trình điều khiển
*   Sử dụng Arduino IDE để viết code cho ESP32 
*   Cấu hình chân điều khiển relay là `OUTPUT`.
*   Triển khai vòng lặp kiểm tra (Test Loop) để đóng/mở relay định kỳ, giúp xác nhận phần cứng đã kết nối đúng.

### 3. Kiểm tra và Cân chỉnh
*   Nạp code lên ESP32 và quan sát trạng thái qua Serial Monitor.
*   Xác nhận relay "nhảy" (nghe tiếng click) và chốt cửa Solenoid co/giãn theo đúng lệnh.
*   Đảm bảo nguồn điện đủ công suất để chốt cửa hoạt động dứt khoát.
