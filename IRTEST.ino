// ---- HC-SR04 Pins ----
const int TRIG_PIN = 8;
const int ECHO_PIN = 9;

// -----------------------

void setup() {
  Serial.begin(57600);     // Match your PCB UART baud rate
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout

  if (duration == 0) return -1.0; // no echo

  return (duration * 0.0343) / 2.0;
}

void loop() {
  float distance = readDistanceCm();

  Serial.println(distance, 1);  // Example: 123.4
  delay(200);                   // 5 readings per second
}