/*
  ESP32 Door Lock Control - Upgraded Version
  Based on example.ino best practices:
  - AsyncWebSocket for better performance
  - WiFi optimization for reliable connection
  - Heartbeat monitoring and safety timeout
  - Auto-lock on connection loss
*/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// ===== HARDWARE CONFIGURATION =====
const int RELAY_PIN = 32;
const int LED_PIN = 2;
bool doorIsOpen = false;

// ===== WIFI CONFIGURATION =====
const char* ssid = "wifi32";
const char* password = "123456789";

// ===== ASYNC WEB SERVER & WEBSOCKET =====
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  // WebSocket at ws://<ip>/ws

// ===== TIMING & SAFETY =====
unsigned long lastWiFiCheck = 0;
unsigned long lastHeartbeat = 0;
const unsigned long WIFI_CHECK_INTERVAL = 5000;
const unsigned long HEARTBEAT_TIMEOUT = 10000;  // Auto-lock after 10s without heartbeat
bool clientConnected = false;

// ===== WIFI OPTIMIZATION =====
void optimizeWiFi() {
    WiFi.setSleep(false);                    // Disable power saving
    WiFi.setTxPower(WIFI_POWER_19_5dBm);     // Maximum TX power
    WiFi.setAutoReconnect(true);             // Auto-reconnect
    WiFi.mode(WIFI_STA);
    Serial.println("WiFi optimized: Sleep OFF, Max TX Power, Auto-Reconnect ON");
}

void monitorWiFiSignal() {
    if (millis() - lastWiFiCheck > WIFI_CHECK_INTERVAL) {
        lastWiFiCheck = millis();
        
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected - reconnecting...");
            WiFi.reconnect();
            return;
        }
        
        int rssi = WiFi.RSSI();
        String signalLevel = (rssi > -50) ? "EXCELLENT" : (rssi > -70) ? "GOOD" : "WEAK";
        
        // Broadcast signal info to all clients
        ws.textAll("SIGNAL:" + signalLevel + ":" + String(rssi));
        Serial.printf("WiFi Signal: %s (%d dBm)\n", signalLevel.c_str(), rssi);
    }
}

// ===== DOOR CONTROL =====
void openDoor() {
    digitalWrite(RELAY_PIN, HIGH);
    doorIsOpen = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Door UNLOCKED (Relay ON)");
}

void lockDoor() {
    digitalWrite(RELAY_PIN, LOW);
    doorIsOpen = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("Door LOCKED (Relay OFF)");
}

// ===== SAFETY CHECK =====
void checkSafetyConditions() {
    // Auto-lock if heartbeat timeout (connection lost)
    if (clientConnected && doorIsOpen && (millis() - lastHeartbeat > HEARTBEAT_TIMEOUT)) {
        Serial.println("SAFETY: Heartbeat timeout - auto-locking door!");
        lockDoor();
        ws.textAll("TIMEOUT:AUTO_LOCKED");
    }
}

// ===== WEBSOCKET MESSAGE HANDLER =====
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        String cmd = "";
        for (size_t i = 0; i < len; i++) {
            cmd += (char)data[i];
        }
        
        // Update heartbeat on any message
        lastHeartbeat = millis();
        
        Serial.println("Received: " + cmd);
        
        // Handle PING/PONG for connection health
        if (cmd == "PING") {
            ws.textAll("PONG");
            return;
        }
        
        // Process door commands
        if (cmd == "OPEN_DOOR") {
            openDoor();
            ws.textAll("OK:Door opened");
            ws.textAll("ACK:OPEN_DOOR");
        } else if (cmd == "LOCK_DOOR") {
            lockDoor();
            ws.textAll("OK:Door locked");
            ws.textAll("ACK:LOCK_DOOR");
        } else if (cmd == "STATUS") {
            String status = doorIsOpen ? "OK:OPEN" : "OK:LOCKED";
            ws.textAll(status);
            ws.textAll("ACK:STATUS");
        }
    }
}

// ===== WEBSOCKET EVENT HANDLER =====
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("Client #%u connected from %s\n", 
                         client->id(), client->remoteIP().toString().c_str());
            clientConnected = true;
            lastHeartbeat = millis();
            
            // Send initial status
            client->text("Connected to Door Lock");
            client->text(doorIsOpen ? "OK:OPEN" : "OK:LOCKED");
            client->text("SIGNAL:GOOD:" + String(WiFi.RSSI()));
            break;
            
        case WS_EVT_DISCONNECT:
            Serial.printf("Client #%u disconnected\n", client->id());
            clientConnected = false;
            break;
            
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
            
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

// ===== SETUP =====
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=============================================");
    Serial.println("ESP32 DOOR LOCK - UPGRADED VERSION");
    Serial.println("=============================================");
    
    // Initialize GPIO
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    
    // Apply WiFi optimizations
    optimizeWiFi();
    
    // Connect to WiFi
    Serial.println("Connecting to WiFi: " + String(ssid));
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n=============================================");
        Serial.println("WiFi CONNECTED!");
        Serial.println("IP Address: " + WiFi.localIP().toString());
        Serial.println("Signal: " + String(WiFi.RSSI()) + " dBm");
        Serial.println("WebSocket: ws://" + WiFi.localIP().toString() + "/ws");
        Serial.println("=============================================");
        
        delay(2000);
    } else {
        Serial.println("\nWiFi connection FAILED!");
        return;
    }
    
    // Default state: locked
    lockDoor();
    
    // Setup WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    
    // HTTP endpoints for backward compatibility
    server.on("/open", HTTP_GET, [](AsyncWebServerRequest *request) {
        openDoor();
        request->send(200, "text/plain", "Door opened");
    });
    
    server.on("/lock", HTTP_GET, [](AsyncWebServerRequest *request) {
        lockDoor();
        request->send(200, "text/plain", "Door locked");
    });
    
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String status = doorIsOpen ? "OPEN" : "LOCKED";
        request->send(200, "text/plain", status);
    });
    
    // Start server
    server.begin();
    Serial.println("Async Web Server started!");
    Serial.println("Door Lock System READY!");
}

// ===== LOOP =====
void loop() {
    ws.cleanupClients();
    monitorWiFiSignal();
    checkSafetyConditions();
    delay(10);
}
