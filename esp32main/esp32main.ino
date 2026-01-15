/**
 * ESP32 Face ID Attendance - Main Controller (Phase 1)
 * Purpose: Door lock control via Relay
 * 
 * NOTE: GPIO 34 is INPUT-ONLY on ESP32. 
 * Switching to GPIO 32 for Relay Output.
 */

const int RELAY_PIN = 32; // Changed from 34 (Input-only) to 32

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure relay is OFF initially
  
  Serial.println("ESP32 Main Controller Initialized.");
  Serial.print("Relay assigned to GPIO: ");
  Serial.println(RELAY_PIN);
}

void loop() {
  // Simple test: Toggle relay every 5 seconds
  Serial.println("Action: Unlocking Door (Relay ON)");
  digitalWrite(RELAY_PIN, HIGH);
  delay(2000); // Keep open for 2 seconds
  
  Serial.println("Action: Locking Door (Relay OFF)");
  digitalWrite(RELAY_PIN, LOW);
  delay(5000); // Wait 5 seconds
}
