HardwareSerial rn2483(1);
String toHex(String input) {
  String hex = "";
  for (int i = 0; i < input.length(); i++) {
    uint8_t c = input[i];
    if (c < 16) hex += "0";
    hex += String(c, HEX);
  }
  hex.toUpperCase();
  return hex;
}

void sendCommand(String cmd) {
  rn2483.print(cmd + "\r\n");
  delay(100);
}

bool sendLoRa(String payload) {
  String hexPayload = toHex(payload);

  rn2483.print("radio tx ");
  rn2483.print(hexPayload);
  rn2483.print("\r\n");

  bool gotOk = false;
  long timeout = millis() + 5000;

  while (millis() < timeout) {
    if (rn2483.available()) {
      String response = rn2483.readStringUntil('\n');
      response.trim();

      Serial.println("RN2483: " + response);

      if (response == "ok") {
        gotOk = true;   // command accepted
      }

      if (response == "radio_tx_ok") {
        return true;    // transmission finished
      }

      if (response == "radio_err") {
        return false;
      }
    }
  }

  return false;
}
#define MUX_A0 5
#define MUX_A1 6
#define MUX_A2 7

// 1-based mux selection (same as your working code)
void selectMux(uint8_t channel) {
  uint8_t c = channel - 1;
  digitalWrite(MUX_A0, c & 0x01);
  digitalWrite(MUX_A1, (c >> 1) & 0x01);
  digitalWrite(MUX_A2, (c >> 2) & 0x01);
  delay(10);
}
String Data[] = {
  "1,2,3",
  "4,5,6",
  "7,8,9",
  "10,11,12"
};

int totalRows = 4;
int currentRow = 0;

void setup() {
  Serial.begin(57600);
  delay(1000);
  rn2483.begin(57600, SERIAL_8N1, 14, 13);

  pinMode(MUX_A0, OUTPUT);
  pinMode(MUX_A1, OUTPUT);
  pinMode(MUX_A2, OUTPUT);

  selectMux(4);   // S4 = Lora_serial
  delay(200);
  sendCommand("radio set mod lora");
  sendCommand("radio set freq 868000000");
  sendCommand("radio set pwr 14");
  sendCommand("radio set sf sf7");
  sendCommand("radio set bw 125");
  sendCommand("radio set cr 4/5");
  sendCommand("radio set sync 12");
  sendCommand("radio set crc on");
  sendCommand("radio set iqi off");
  sendCommand("radio set preamble 8");
  Serial.println("Setup complete");
  Serial.println("Sending over Lora (S4)...");
  delay(3000);
}


void loop() {
  if (currentRow < totalRows) {

    String row = Data[currentRow];

    Serial.print("Sending row: ");
    Serial.println(row);

    bool success = sendLoRa(row + "\n");

    if (success) {
      Serial.println("Sent OK");
      currentRow++;
    } else {
      Serial.println("Send failed, retrying...");
    }

    delay(1000); // spacing between transmissions
  }
}
