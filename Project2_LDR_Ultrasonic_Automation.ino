Project 2: Smart Object Detection and Environment Control System 
Using LDR and Ultrasonic Sensor 
// LED + Fan control using LDR (auto-calibration) + Ultrasonic Sensor 
// Conditions: 
// DARK + Object detected → LED ON + FAN ON 
// Pins: 
// A0  -> LDR voltage divider 
// D3  -> Ultrasonic TRIG 
// D4  -> Ultrasonic ECHO 
// D8  -> Fan control (via transistor) 
// D9  -> LED 
const int ldrPin = A0; 
const int ledPin = 9; 
const int fanPin = 8; 
// Ultrasonic pins 
const int trigPin = 3; 
const int echoPin = 4; 
// ---------- LDR AUTO CALIBRATION ---------- 
const unsigned long calibMillis = 5000UL; 
int minLdr = 1023; 
int maxLdr = 0; 
int threshold = 300; 
// ---------- SMOOTHING ---------- 
const int SMOOTH_N = 6; 
int smoothBuf[SMOOTH_N]; 
int smoothIdx = 0; 
long smoothSum = 0; 
// ---------- ULTRASONIC ---------- 
const int DIST_THRESHOLD = 10; // cm (object detected within 30cm) 
void setup() { 
Serial.begin(9600); 
pinMode(ledPin, OUTPUT); 
pinMode(fanPin, OUTPUT); 
pinMode(trigPin, OUTPUT); 
pinMode(echoPin, INPUT); 
digitalWrite(ledPin, LOW); 
digitalWrite(fanPin, LOW); 
// Initialize smoothing buffer 
for (int i = 0; i < SMOOTH_N; i++) { 
smoothBuf[i] = analogRead(ldrPin); 
smoothSum += smoothBuf[i]; 
} 
// -------- AUTO CALIBRATION -------- 
Serial.println("Auto-calibrating LDR for 5 seconds..."); 
unsigned long start = millis(); 
while (millis() - start < calibMillis) { 
int v = analogRead(ldrPin); 
if (v < minLdr) minLdr = v; 

if (v > maxLdr) maxLdr = v; 
delay(200); 
} 
threshold = (minLdr + maxLdr) / 2; 
Serial.println("Calibration Done"); 
Serial.print("LDR Threshold = "); 
Serial.println(threshold); 
} 
void loop() { 
// -------- LDR READ & SMOOTH -------- 
int raw = analogRead(ldrPin); 
smoothSum -= smoothBuf[smoothIdx]; 
smoothBuf[smoothIdx] = raw; 
smoothSum += raw; 
smoothIdx = (smoothIdx + 1) % SMOOTH_N; 
int ldrValue = smoothSum / SMOOTH_N; 
bool isDark = (ldrValue < threshold); 
// -------- ULTRASONIC READ -------- 
digitalWrite(trigPin, LOW); 
delayMicroseconds(2); 
digitalWrite(trigPin, HIGH); 
delayMicroseconds(10); 
digitalWrite(trigPin, LOW); 
long duration = pulseIn(echoPin, HIGH, 25000); // timeout 
int distance = duration * 0.034 / 2; 
bool objectDetected = (distance > 0 && distance <= DIST_THRESHOLD); 
// -------- CONTROL LOGIC -------- 
if (isDark && objectDetected) { 
digitalWrite(ledPin, HIGH);   // LED ON 
digitalWrite(fanPin, HIGH);   // FAN ON 
} else { 
digitalWrite(ledPin, LOW);    // LED OFF 
digitalWrite(fanPin, LOW);    // FAN OFF 
} 
// -------- SERIAL MONITOR -------- 
Serial.print("LDR="); 
Serial.print(ldrValue); 
Serial.print(" | Dark="); 
Serial.print(isDark ? "YES" : "NO"); 
Serial.print(" | Distance="); 
Serial.print(distance); 
Serial.print("cm"); 
Serial.print(" | Object="); 
Serial.print(objectDetected ? "YES" : "NO"); 
Serial.print(" | LED="); 
Serial.print(digitalRead(ledPin) ? "ON" : "OFF"); 
Serial.print(" | FAN="); 
Serial.println(digitalRead(fanPin) ? "ON" : "OFF"); 
delay(500); 
}
