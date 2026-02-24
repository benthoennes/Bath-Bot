#define MUX_A0 5
#define MUX_A1 6
#define MUX_A2 7

HardwareSerial uart1(1);

// 1-based mux selection (same as your working code)
void selectMux(uint8_t channel) {
  uint8_t c = channel - 1;
  digitalWrite(MUX_A0, c & 0x01);
  digitalWrite(MUX_A1, (c >> 1) & 0x01);
  digitalWrite(MUX_A2, (c >> 2) & 0x01);
  delay(10);
}

void setup() {
  Serial.begin(57600);   
  delay(3000);                      // USB serial monitor
  uart1.begin(57600, SERIAL_8N1, 14, 13);      // RX=14, TX=13

  pinMode(MUX_A0, OUTPUT);
  pinMode(MUX_A1, OUTPUT);
  pinMode(MUX_A2, OUTPUT);

  selectMux(2);   // S2 = Sonar UART
  delay(200);

  Serial.println("Listening to Sonar UART (S2)...");
}

void loop() {
  if (uart1.available()) {
    String line = uart1.readStringUntil('\n');
    line.trim();

    Serial.print("Received: ");
    Serial.println(line);
  }
}