#define MUX_A0 5
#define MUX_A1 6
#define MUX_A2 7

HardwareSerial rn2483(1);

String fromHex(String hex) {
  String result = "";

  for (int i = 0; i < hex.length(); i += 2) {
    String byteStr = hex.substring(i, i + 2);
    char c = (char) strtol(byteStr.c_str(), NULL, 16);
    result += c;
  }

  return result;
}

void sendCommand(String cmd) {
  rn2483.print(cmd + "\r\n");
  delay(100);
}

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
  delay(1000);                      // USB serial monitor
  rn2483.begin(57600, SERIAL_8N1, 14, 13);      // RX=14, TX=13
  delay(1000);
  pinMode(MUX_A0, OUTPUT);
  pinMode(MUX_A1, OUTPUT);
  pinMode(MUX_A2, OUTPUT);

  selectMux(4);   //Lora
  delay(500);
  sendCommand("sys reset");
  delay(1000);

  sendCommand("mac pause");   // 🔥 REQUIRED

  sendCommand("radio set mod lora");
  sendCommand("radio set freq 868000000");
  sendCommand("radio set sf sf7");
  sendCommand("radio set bw 125");
  sendCommand("radio set cr 4/5");
  sendCommand("radio set pwr 14");
  sendCommand("radio set wdt 0");

  sendCommand("radio set crc on");
  sendCommand("radio set sync 12");
  sendCommand("radio set iqi off");

  delay(200);

  sendCommand("radio rx 0");
  Serial.println("Setup complete");
  Serial.println("Listening Over Lora UART (S4)...");
}

void loop() {
  
  if (rn2483.available()) {
    String response = rn2483.readStringUntil('\n');
    response.trim();

    Serial.println("RN2483: " + response);

    if (response.startsWith("radio_rx ")) {
      String hexPayload = response.substring(9);

      String decoded = fromHex(hexPayload);

      Serial.print("Received: ");
      Serial.println(decoded);

      // IMPORTANT: restart RX
      sendCommand("radio rx 0");
    }

    if (response == "radio_err") {
      Serial.println("RX timeout, restarting...");
      sendCommand("radio rx 0");
    }
    if (response == "busy") {
      Serial.println("Radio busy → restarting RX");
      sendCommand("radio stop");
      delay(50);
      sendCommand("radio rx 0");
    }
  }
  
}