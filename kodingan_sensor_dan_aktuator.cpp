#include <Arduino.h> // Wajib untuk PlatformIO

// PIN SENSOR
const int PIN_RAIN_SENSOR = 4;   // Sensor Hujan (Digital Output / DO)
const int PIN_US_TRIG = 5;       // Ultrasonik - Trigger
const int PIN_US_ECHO = 6;       // Ultrasonik - Echo
const int PIN_FLOAT_SWITCH = 7;  // Float Switch (Pelampung Darurat)
const int PIN_FLOW_SENSOR = 15;  // Water Flow Sensor (Debit Air)

// PIN AKTUATOR (Relay Pompa & Solenoid Valve)
const int PIN_RELAY_PUMP = 16;   // Pompa Air
const int PIN_RELAY_VALVE = 17;  // Katup / Solenoid Valve

const float TINGGI_RESERVOIR = 100.0;
const float BATAS_MIN_POMPA = 20.0;
const float BATAS_KRITIS_VALVE = 80.0;

float tinggiAir = 0.0;
int statusFloat = HIGH;

void baca_ultrasonik();
void Pembacaan_Status_Hujan();
void batas_air();

void setup() {
  Serial.begin(115200);  // Baud rate standar ESP32-S3

  pinMode(PIN_RAIN_SENSOR, INPUT);
  pinMode(PIN_US_ECHO, INPUT);
  pinMode(PIN_FLOAT_SWITCH, INPUT_PULLUP);
  pinMode(PIN_FLOW_SENSOR, INPUT_PULLUP);

  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_RELAY_PUMP, OUTPUT);
  pinMode(PIN_RELAY_VALVE, OUTPUT);

  digitalWrite(PIN_US_TRIG, LOW);
  digitalWrite(PIN_RELAY_PUMP, LOW);
  digitalWrite(PIN_RELAY_VALVE, LOW);
}

void loop() {
  baca_ultrasonik();
  Pembacaan_Status_Hujan();
  batas_air();
  
  delay(200);
}

void baca_ultrasonik() {
  digitalWrite(PIN_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);

  long durasi = pulseIn(PIN_US_ECHO, HIGH, 30000);
  float jarakAir = (durasi * 0.034) / 2.0;
  tinggiAir = TINGGI_RESERVOIR - jarakAir;
}

void Pembacaan_Status_Hujan() {
  int statusHujan = digitalRead(PIN_RAIN_SENSOR);
  int statusFloat = digitalRead(PIN_FLOAT_SWITCH);

  if (statusHujan == LOW && statusFloat == LOW) {
    digitalWrite(PIN_RELAY_PUMP, HIGH);
    digitalWrite(PIN_RELAY_VALVE, HIGH); 
  } else {
    digitalWrite(PIN_RELAY_PUMP, LOW);
    digitalWrite(PIN_RELAY_VALVE, LOW);
  }
}

void batas_air() {
  if (tinggiAir >= BATAS_KRITIS_VALVE || statusFloat == LOW) {
    digitalWrite(PIN_RELAY_PUMP, LOW);
  } else if (tinggiAir >= BATAS_MIN_POMPA) {
    digitalWrite(PIN_RELAY_PUMP, HIGH);
    digitalWrite(PIN_RELAY_VALVE, LOW);
  } else {
    digitalWrite(PIN_RELAY_PUMP, LOW);
    digitalWrite(PIN_RELAY_VALVE, LOW);
  }
}
