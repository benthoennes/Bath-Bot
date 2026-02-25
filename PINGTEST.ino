#include "ping1d.h"
#define MUX_A0 5
#define MUX_A1 6
#define MUX_A2 7

HardwareSerial PingSerial(1);
// Create Ping1D object
Ping1D ping(PingSerial);

// Use UART2 on ESP32
void selectMux(uint8_t channel) {
  uint8_t c = channel - 1;
  digitalWrite(MUX_A0, c & 0x01);
  digitalWrite(MUX_A1, (c >> 1) & 0x01);
  digitalWrite(MUX_A2, (c >> 2) & 0x01);
  delay(10);
}


void setup() {
  
  pinMode(MUX_A0, OUTPUT);
  pinMode(MUX_A1, OUTPUT);
  pinMode(MUX_A2, OUTPUT);

  selectMux(2);   // S2 = Sonar UART
  delay(200);

  Serial.begin(115200);
  delay(1000);

  Serial.println("Ping2 depth reader starting");

  // RX = GPIO 16, TX = GPIO 17
  PingSerial.begin(115200, SERIAL_8N1, 14, 13);


  // Initialize Ping1D
  ping.initialize();
}

void loop() {
  // Update data from Ping2
  if (ping.update()) {
    // Distance is in millimeters
    float depth_m = ping.distance() / 1000.0;

    Serial.print("Depth: ");
    Serial.print(depth_m, 3);
    Serial.println(" m");
  } else {
    Serial.println("Ping2 update failed");
  }

  delay(500);
}
