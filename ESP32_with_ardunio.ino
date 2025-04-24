// #include <WiFi.h>
// #include <HTTPClient.h>

// HardwareSerial ArduinoSerial(1); // Use UART1 (RX=16, TX=17)

// const char* ssid = "POCO";
// const char* password = "9691073541";
// const String serverUrl = "http://192.168.124.200:4000/api/sensor/getSensorData";

// void setup() {
//   Serial.begin(115200); // For debug monitor
//   ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17); // UART1

//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\n✅ Connected to WiFi");
// }

// void loop() {
//   if (WiFi.status() == WL_CONNECTED && ArduinoSerial.available()) {
//     String sensorData = ArduinoSerial.readStringUntil('\n');
//     sensorData.trim();

//     // ✅ Check if string has 3 commas (i.e. 4 values)
//     int commaCount = 0;
//     for (int i = 0; i < sensorData.length(); i++) {
//       if (sensorData.charAt(i) == ',') commaCount++;
//     }

//     if (commaCount == 3) {
//       Serial.println("📥 Received valid data: " + sensorData);

//       // Split values
//       int first = sensorData.indexOf(',');
//       int second = sensorData.indexOf(',', first + 1);
//       int third = sensorData.indexOf(',', second + 1);

//       float temperature = sensorData.substring(0, first).toFloat();
//       float humidity = sensorData.substring(first + 1, second).toFloat();
//       int waterDetected = sensorData.substring(second + 1, third).toInt();
//       int flameDetected = sensorData.substring(third + 1).toInt();

//       // JSON payload
//       String json = "{\"temperature\":" + String(temperature) +
//                     ",\"humidity\":" + String(humidity) +
//                     ",\"waterDetected\":" + String(waterDetected) +
//                     ",\"flameDetected\":" + String(flameDetected) + "}";

//       HTTPClient http;
//       http.begin(serverUrl);
//       http.addHeader("Content-Type", "application/json");
//       int code = http.POST(json);

//       if (code > 0) {
//         String res = http.getString();
//         Serial.println("✅ Sent to server: " + res);
//       } else {
//         Serial.println("❌ Failed to send. HTTP code: " + String(code));
//       }

//       http.end();
//     } else {
//       Serial.println("⚠️ Ignored non-sensor message: " + sensorData);
//     }
//   }

//   delay(1000);
// }



#define BLYNK_TEMPLATE_ID "TMPL3XN0NEgHt"
#define BLYNK_TEMPLATE_NAME "MOTOR CONTROL"
#define BLYNK_AUTH_TOKEN "wISauJLuCxAWeZU1sXQ8LH2dUnPV1zK7"

#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

// Use UART1 for communication with Arduino (RX=16, TX=17)
HardwareSerial ArduinoSerial(1);

const char* ssid = "POCO";
const char* password = "9691073541";
const String serverUrl = "http://192.168.124.200:4000/api/sensor/getSensorData";

char auth[] = BLYNK_AUTH_TOKEN;

void setup() {
  Serial.begin(115200);                          // For debugging
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17); // UART1 for Arduino
  Blynk.begin(auth, ssid, password);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi");
}

// Blynk Control Handlers
BLYNK_WRITE(V0) {
  int value = param.asInt();
  ArduinoSerial.println("DCMOTOR:" + String(value));
  // Serial.println("Sending to Arduino: DCMOTOR:" + String(value)); // Add in V0

}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  ArduinoSerial.println("FAN:" + String(value));
  // Serial.println("Sending to Arduino: FAN:" + String(value)); // Add in V0

}

BLYNK_WRITE(V2) {
  int value = param.asInt();
  ArduinoSerial.println("PUMP:" + String(value));
  // Serial.println("Sending to Arduino: PUMP:" + String(value)); // Add in V0

}

// BLYNK_WRITE(V3) {
//   int angle = param.asInt();
//   ArduinoSerial.println("SERVO:" + String(angle));
//   Serial.println("Sending to Arduino: SERVO:" + String(angle)); // Add in V0

// }

void loop() {
  Blynk.run();

  // Read sensor data from Arduino and send to backend server
  if (WiFi.status() == WL_CONNECTED && ArduinoSerial.available()) {
    String sensorData = ArduinoSerial.readStringUntil('\n');
    sensorData.trim();

    // Check if valid data (3 commas means 4 values)
    int commaCount = 0;
    for (int i = 0; i < sensorData.length(); i++) {
      if (sensorData.charAt(i) == ',') commaCount++;
    }

    if (commaCount == 3) {
      Serial.println("📥 Received valid data: " + sensorData);

      // Parse values
      int first = sensorData.indexOf(',');
      int second = sensorData.indexOf(',', first + 1);
      int third = sensorData.indexOf(',', second + 1);

      float temperature = sensorData.substring(0, first).toFloat();
      float humidity = sensorData.substring(first + 1, second).toFloat();
      int waterDetected = sensorData.substring(second + 1, third).toInt();
      int flameDetected = sensorData.substring(third + 1).toInt();

      // Create JSON payload
      String json = "{\"temperature\":" + String(temperature) +
                    ",\"humidity\":" + String(humidity) +
                    ",\"waterDetected\":" + String(waterDetected) +
                    ",\"flameDetected\":" + String(flameDetected) + "}";

      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");
      int code = http.POST(json);

      if (code > 0) {
        String res = http.getString();
        Serial.println("✅ Sent to server: " + res);
      } else {
        Serial.println("❌ Failed to send. HTTP code: " + String(code));
      }

      http.end();
    } else {
      Serial.println("⚠️ Ignored non-sensor message: " + sensorData);
    }
  }

  delay(1000); // Adjust delay as needed
}
