# ===== CẤU HÌNH SERVER (LAPTOP) =====
SERVER_HOST = "0.0.0.0"         # Lắng nghe trên tất cả card mạng
SERVER_PORT = 8000              

# ===== CẤU HÌNH ESP32 (Lấy từ Hotspot Clients list) =====
# Cần kiểm tra IP của các ESP32 trong phần Mobile Hotspot của Laptop
ESP32_MAIN_IP = "192.168.137.122"  # Thay bằng IP thực của ESP32 Main
ESP32_MAIN_PORT = 80            
ESP32_CAM_IP = "192.168.137.51"   # Thay bằng IP thực của ESP32-CAM
ESP32_CAM_STREAM = f"http://{ESP32_CAM_IP}/stream"

# ===== CẤU HÌNH LOGGING =====
LOG_LEVEL = "INFO"
LOG_FILE = "logs/server.log"
