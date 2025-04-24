// #include <DHT.h>
// #include <Servo.h>
// #include <LiquidCrystal.h>

// #define DHTPIN 2
// #define DHTTYPE DHT11
// #define WATER_SENSOR A0
// #define FLAME_SENSOR A1
// #define DC_MOTOR 3
// #define FAN 4
// #define SERVO_PIN 5
// #define WATER_PUMP 6
// #define TRIG_PIN 7
// #define ECHO_PIN 8
// #define BUZZER 9

// // LCD Pins: RS, E, D4, D5, D6, D7
// LiquidCrystal lcd(10, 11, 12, 13, A2, A3);

// DHT dht(DHTPIN, DHTTYPE);
// Servo servo;

// void setup() {
//   Serial.begin(9600);
//   dht.begin();
//   servo.attach(SERVO_PIN);

//   pinMode(WATER_SENSOR, INPUT);
//   pinMode(FLAME_SENSOR, INPUT);
//   pinMode(DC_MOTOR, OUTPUT);
//   pinMode(FAN, OUTPUT);
//   pinMode(WATER_PUMP, OUTPUT);
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
//   pinMode(BUZZER, OUTPUT);

//   lcd.begin(16, 2);
//   lcd.print("Initializing...");
// }

// void loop() {
//   float temperature = dht.readTemperature();
//   float humidity = dht.readHumidity();
//   int waterDetected = digitalRead(WATER_SENSOR);
//   int flameDetected = digitalRead(FLAME_SENSOR);

//   long duration, distance;
//   digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);
//   duration = pulseIn(ECHO_PIN, HIGH);
//   distance = duration * 0.034 / 2;


//   if (Serial.available()) {
//   String command = Serial.readStringUntil('\n');
//   command.trim();
//   Serial.println("📥 Received: " + command);

//   if (command.startsWith("DCMOTOR:")) {
//     int state = command.substring(8).toInt();
//     digitalWrite(DC_MOTOR, state);
//     Serial.println("✅ DC Motor set to: " + String(state));
//   } else if (command.startsWith("FAN:")) {
//     int state = command.substring(4).toInt();
//     digitalWrite(FAN, state == 1 ? LOW : HIGH);
//     Serial.println("✅ Fan set to: " + String(state));
//   } else if (command.startsWith("PUMP:")) {
//     int state = command.substring(5).toInt();
//     digitalWrite(WATER_PUMP, state == 1 ? LOW : HIGH);
//     Serial.println("✅ Water pump set to: " + String(state));
//   }
// }





//   // DC Motor ON if Temp > 35
//   digitalWrite(DC_MOTOR, temperature > 35);

//   // Fan ON if Humidity > 60
//   // digitalWrite(FAN, humidity > 60);
//   digitalWrite(FAN, humidity < 60 ? HIGH : LOW);

//   // Servo ON if Water (Rain) detected
//   if (waterDetected == HIGH) servo.write(90);
//   else servo.write(0);

//   // Water Pump ON if Flame detected (LOW signal)
//   digitalWrite(WATER_PUMP, flameDetected == LOW ? LOW : HIGH);

//   // Buzzer ON if object < 20cm
//   digitalWrite(BUZZER, distance < 20 ? HIGH : LOW);

//   // Display on LCD
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("T:");
//   lcd.print(temperature);
//   lcd.print("C H:");
//   lcd.print(humidity);
//   lcd.print("%");

//   lcd.setCursor(0, 1);
//   lcd.print("W:");
//   lcd.print(waterDetected);
//   lcd.print(" F:");
//   lcd.print(flameDetected);

//   // Send to ESP32
//   if (!isnan(temperature) && !isnan(humidity)) {
//     Serial.print(temperature); Serial.print(",");
//     Serial.print(humidity); Serial.print(",");
//     Serial.print(waterDetected); Serial.print(",");
//     Serial.println(flameDetected); // ends with '\n'
//   }

//   delay(2000);
// }


#include <DHT.h>
#include <Servo.h>
#include <LiquidCrystal.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define WATER_SENSOR A0
#define FLAME_SENSOR A1
#define DC_MOTOR 3
#define FAN 4
#define SERVO_PIN 5
#define WATER_PUMP 6
#define TRIG_PIN 7
#define ECHO_PIN 8
#define BUZZER 9

LiquidCrystal lcd(10, 11, 12, 13, A2, A3);

DHT dht(DHTPIN, DHTTYPE);
Servo servo;

// Manual control flags
bool manualFan = false;
bool manualPump = false;
bool manualDCMotor = false;

void setup() {
  Serial.begin(9600);
  dht.begin();
  servo.attach(SERVO_PIN);

  pinMode(WATER_SENSOR, INPUT);
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(DC_MOTOR, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Initializing...");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int waterDetected = digitalRead(WATER_SENSOR);
  int flameDetected = digitalRead(FLAME_SENSOR);

  long duration, distance;
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // 📥 Read Serial Command
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.println("📥 Received: " + command);

    if (command.startsWith("DCMOTOR:")) {
      String value = command.substring(8);
      if (value == "AUTO") {
        manualDCMotor = false;
        // Serial.println("🔁 DC Motor set to AUTO mode");
      } else {
        int state = value.toInt();
        digitalWrite(DC_MOTOR, state);
        manualDCMotor = true;
        // Serial.println("✅ DC Motor set to: " + String(state));
      }
    }
    else if (command.startsWith("FAN:")) {
      String value = command.substring(4);
      if (value == "AUTO") {
        manualFan = false;
        // Serial.println("🔁 Fan set to AUTO mode");
      } else {
        int state = value.toInt();
        digitalWrite(FAN, state == 1 ? LOW : HIGH); // relay logic
        manualFan = true;
        // Serial.println("✅ Fan set to: " + String(state));
      }
    }
    else if (command.startsWith("PUMP:")) {
      String value = command.substring(5);
      if (value == "AUTO") {
        manualPump = false;
        // Serial.println("🔁 Pump set to AUTO mode");
      } else {
        int state = value.toInt();
        digitalWrite(WATER_PUMP, state == 1 ? LOW : HIGH); // relay logic
        manualPump = true;
        // Serial.println("✅ Water pump set to: " + String(state));
      }
    }
  }

  // ✅ AUTOMATIC CONTROL if manual mode is OFF
  if (!manualDCMotor) {
    digitalWrite(DC_MOTOR, temperature > 35);
  }

  if (!manualFan) {
    digitalWrite(FAN, humidity < 60 ? HIGH : LOW); // relay active LOW
  }

  if (!manualPump) {
    digitalWrite(WATER_PUMP, flameDetected == LOW ? LOW : HIGH); // relay active LOW
  }

  // Servo ON if Water (Rain) detected
  servo.write(waterDetected == HIGH ? 90 : 0);

  // Buzzer ON if object < 20cm
  digitalWrite(BUZZER, distance < 20 ? HIGH : LOW);

  // LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("W:");
  lcd.print(waterDetected);
  lcd.print(" F:");
  lcd.print(flameDetected);

  // Send to ESP32
  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.print(temperature); Serial.print(",");
    Serial.print(humidity); Serial.print(",");
    Serial.print(waterDetected); Serial.print(",");
    Serial.println(flameDetected);
  }

  delay(2000);
}







