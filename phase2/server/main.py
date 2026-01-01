from flask import Flask, render_template, Response
from flask_socketio import SocketIO, emit
import cv2
import threading
import time
import os
import websocket
from websocket import create_connection
import logging
from config import *

# Xác định thư mục cơ sở
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Tạo thư mục logs nếu chưa có
LOG_DIR = os.path.join(BASE_DIR, 'logs')
if not os.path.exists(LOG_DIR):
    os.makedirs(LOG_DIR)

LOG_PATH = os.path.join(LOG_DIR, 'server.log')

# Cấu hình logging
logging.basicConfig(level=LOG_LEVEL, filename=LOG_PATH, 
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Khởi tạo Flask & SocketIO
app = Flask(__name__, 
            static_folder='static',
            template_folder='static') # Dùng static làm template folder để map index.html
socketio = SocketIO(app, cors_allowed_origins="*")

# ================= DOOR LOCK CONNECTION (WebSocket Client) =================
lock_ws = None
lock_connected = False
ROBOT_WS_URL = f"ws://{ESP32_MAIN_IP}/ws"

def connect_to_lock():
    global lock_ws, lock_connected
    try:
        lock_ws = create_connection(ROBOT_WS_URL, timeout=3)
        lock_connected = True
        logger.info(f"✅ Connected to Door Lock at {ROBOT_WS_URL}")
        print(f"✅ Connected to Door Lock at {ROBOT_WS_URL}")
        # Broadcast connection status to web clients
        socketio.emit('response', 'LOCK_CONNECTED')
        return True
    except Exception as e:
        logger.error(f"⚠️ Could not connect to Door Lock: {e}")
        lock_ws = None
        lock_connected = False
        return False

def lock_listener():
    """Background thread to listen for messages from ESP32 and broadcast to web clients"""
    global lock_ws, lock_connected
    while True:
        if lock_ws and lock_connected:
            try:
                # Set a short timeout so we can check connection periodically
                lock_ws.settimeout(1.0)
                message = lock_ws.recv()
                if message:
                    logger.info(f"Door Lock says: {message}")
                    print(f"🔔 Door Lock: {message}")
                    # Broadcast to all web clients
                    socketio.emit('response', message)
            except websocket.WebSocketTimeoutException:
                # Timeout is normal, continue listening
                pass
            except Exception as e:
                logger.error(f"Lost connection to Door Lock: {e}")
                print(f"❌ Lost connection to Door Lock: {e}")
                lock_ws = None
                lock_connected = False
                socketio.emit('response', 'LOCK_DISCONNECTED')
        else:
            # Try to reconnect
            if not lock_connected:
                connect_to_lock()
            time.sleep(3)

# Start listener thread
threading.Thread(target=lock_listener, daemon=True).start()

# ================= VIDEO PROCESSING (OpenCV) =================
def generate_frames():
    cap = cv2.VideoCapture(ESP32_CAM_STREAM)
    while True:
        success, frame = cap.read()
        if not success:
            logger.warning("Camera stream disconnected, retrying...")
            cap.open(ESP32_CAM_STREAM)
            time.sleep(1)
            continue

        ret, buffer = cv2.imencode('.jpg', frame)
        if not ret:
            continue
            
        frame_bytes = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

# ================= ROUTES =================
@app.route('/')
def index():
    # Flask uses render_template to serve HTML
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# ================= SOCKET.IO EVENTS =================
@socketio.on('connect')
def handle_connect():
    logger.info("Client connected to Flask-SocketIO")
    # Send current lock connection status to the new client
    if lock_connected:
        emit('response', 'LOCK_CONNECTED')
    else:
        emit('response', 'LOCK_DISCONNECTED')

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected from Flask-SocketIO")

@socketio.on('command')
def handle_command(data):
    global lock_ws, lock_connected
    logger.info(f"Received command: {data}")
    
    if lock_ws and lock_connected:
        try:
            lock_ws.send(data)
            emit('response', f"OK: Sent {data} to Door Lock")
        except Exception as e:
            logger.error(f"Error sending command: {e}")
            lock_ws = None
            emit('response', "ERROR: Door Lock connection lost")
    else:
        emit('response', "ERROR: Door Lock not connected")

# ================= MAIN =================
if __name__ == "__main__":
    print("=" * 50)
    print("🚀 ESP32 Door Lock Control Server")
    print("=" * 50)
    print(f"📡 Web Interface: http://localhost:{SERVER_PORT}")
    print(f"📡 Video Feed:    http://localhost:{SERVER_PORT}/video_feed")
    print(f"🔌 Door Lock WS:  {ROBOT_WS_URL}")
    print("=" * 50)
    logger.info(f"Starting Flask server at http://{SERVER_HOST}:{SERVER_PORT}")
    socketio.run(app, host=SERVER_HOST, port=SERVER_PORT, debug=False)
