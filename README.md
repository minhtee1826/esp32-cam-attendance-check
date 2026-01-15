# ESP32 Face ID Attendance & Security System

## 🎯 PROJECT GOALS

Build a smart security control panel system integrated with face recognition attendance:
- **Security Panel**: Central control panel for offices/homes.
- **Face ID**: Real-time face recognition using ESP32-CAM and a Python Server (YOLO/OpenCV).
- **GUI & Navigation**: Intuitive user interface on an LCD screen.
- **Access Control**: ESP32 controlling door lock latch via Relay.
- **Connectivity**: System and attendance database accessible via WebSocket from the Python Server.

## 📁 PROJECT STRUCTURE (Planned)

```
esp32_attendance/
├── esp32_main/              # Code for ESP32 controlling Relay & LCD
│   ├── esp32_main.ino
│   └── components/
├── esp32_cam/               # Code for ESP32-CAM video streaming
│   └── esp32_cam.ino
├── python_server/           # Face Recognition Server & Database
│   ├── app/
│   │   ├── main.py
│   │   ├── face_recog.py    # Face recognition logic
│   │   ├── websocket.py     # WebSocket connection management
│   │   └── database.py      # Attendance management
│   ├── requirements.txt
│   └── .env
├── hardware/                # Schematic & PCB drawings
└── README.md
```

## 🛠️ HARDWARE COMPONENT LIST

| Component | Quantity | Status | Notes |
| :--- | :---: | :---: | :--- |
| **ESP32 DevKit V1** | 01 | ✅ | Main controller, Relay, LCD |
| **ESP32-CAM** | 01 | ✅ | Video stream & Face ID |
| **LCD 16x2 / 20x4 I2C** | 01 | ⏳ | Menu & status display |
| **Solenoid Door Lock** | 01 | ✅ | Check power & suction force |
| **5V Relay Module** | 01 | ⏳ | Trigger door latch |
| **12V Switching Power Supply** | 01 | ✅ | Main power supply for system |
| **RFID Scanner (RC522)** | 03 | ⏳ | Backup door opening via card |
| **RFID Tag / Keycard** | - | ⏳ | Included accessories |
| **DC-to-DC Converter (12V->5V)** | 02 | ✅ | Step-down for ESP32 & Cam |
| **Buzzer** | 01 | ✅ | Status alerts |
| **Fan & Light** | 01 | ✅ | Supporting peripherals |

## 📋 DETAILED TASK LIST

### 🛠️ Hardware Phase
- [ ] Verify components: Switching power supply, ESP32, ESP32-CAM, Relay, Door latch.
- [ ] Design and install Schematic.
- [ ] Solder board and 3D print enclosure (Hardware Final).

### 💻 Software Development Phase
- **Phase 1**: Control door latch via ESP32 and Relay.
- **Phase 2**: Setup raw WebSocket for video streaming and functional testing.
- **Phase 3**: Implement Face ID (Python Server side).
- **Phase 4**: Connect ESP32-CAM with ESP32 to trigger door opening on successful recognition.
- **Phase 5**: Finalize Web interface (Functional & Aesthetic GUI).
- **Phase 6**: Integrate sensors and peripherals (Automatic Fan, Lights).

## ⚙️ IMPLEMENTATION WORKFLOW

### Stage 0: Components & Schematic
- ✅ Check Switching Power Supply (Confirmed working)
- ✅ Check ESP32 (Confirmed working)
- ✅ Check ESP32-CAM (Confirmed working)
- ⏳ Check Door Latch

### Stage 1: ESP32 to Door Connection
- Connect ESP32 to Relay.
- Implement basic door open/close mechanism logic.

### Stage 2: WebSocket Foundation & Video Transmission
- **Stream ESP32-CAM**: Establish stable MJPEG video stream from ESP32-CAM.
- **WebSocket Server Setup**: Launch Python-based WebSocket server as a data coordination hub.
- **Raw GUI**: Build a basic Web interface to display video stream and manual override buttons.
- **Communication Test**: Verify packet transmission between Web, Server, and hardware devices.

### Stage 3: Face ID Engine & Database Integration
- **AI Model Implementation**: Integrate AI models (YOLO/Face Recognition) on the Python Server for image frame processing.
- **Attendance Database**: Setup database (SQLite/PostgreSQL) to manage user lists and attendance history.
- **Real-time Recognition**: Match faces from stream against the Database to identify individuals (Success/Fail/Stranger).
- **Security Logic**: Build security rules (e.g., only open door when Face ID + Database match).

### Stage 4: System Integration & Automatic Control
- **Cross-Device Coordination**: Link data flow: ESP32-CAM (Input) -> Python Server (AI) -> ESP32 Main (Relay Control).
- **Relay Automation**: Automatically send door latch open command via WebSocket to ESP32 upon successful recognition.
- **LCD Feedback**: Display user information and door status (Welcome/Access Denied) on the LCD screen connected to the ESP32.
- **Full System GUI**: Upgrade Web interface to display real-time attendance logs and manage user accounts.

### Stage 5 & 6: Polishing & Expansion
- **Refine Web GUI**: Complete aesthetic, responsive Web interface.
- **Peripheral Automation**: Automate fan, lights, and other external devices.

### Stage 7: Hardware Final (3D Design & Enclosure)
- **CAD Design**: Design custom enclosure to house LCD screen, ESP32-CAM, and circuit boards.
- **Space Optimization**: Optimize space for wiring and cooling solutions.
- **3D Printing**: Proceed with 3D printing parts using plastic (PLA/PETG) and full mechanical assembly.
- **Mounting**: Design brackets and mounting frames for the Solenoid door latch.

## 🔧 INSTALLATION AND RUNNING

### 1. ESP32 & ESP32-CAM
- Upload code using Arduino IDE.
- Configure WiFi SSID and Password in the source code.

### 2. Python Server
```cmd
cd python_server
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
python main.py
```

## 📊 PERFORMANCE TARGETS

- **Latency (Face ID to Door)**: ≤ 1.5s (Typical)
- **Video Stream FPS**: 15-25 FPS (Depending on WiFi bandwidth)
- **Recognition Accuracy**: ≥ 95% under good lighting conditions
- **WebSocket Latency**: ≤ 100ms for control commands
- **Reconnect Time**: 2-5s upon WiFi/Server connection loss
- **System Uptime**: 24/7 with self-healing mechanisms

## 🧪 ACCEPTANCE CRITERIA

- [ ] Stable video stream from ESP32-CAM.
- [ ] Accurate face recognition (Face ID).
- [ ] Automatic door unlocking on successful recognition.
- [ ] Attendance data stored and accessible via Web.
- [ ] Smooth operation of LCD and Web interfaces.

## 🚨 IMPORTANT NOTES

1. **Power Supply**: Use a stable switching power supply to power the door latch and ESP32.
2. **Security**: Secure WebSocket connections and the database.
3. **Performance**: Optimize AI models to run smoothly on the Python Server.
4. **Enclosure**: Ensure good heat dissipation for the ESP32 and ESP32-CAM.
