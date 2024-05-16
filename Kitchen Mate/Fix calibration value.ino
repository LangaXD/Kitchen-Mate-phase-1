#include <HX711_ADC.h>
#include <BlynkSimpleEsp8266.h>

const int HX711_dout = 4;
const int HX711_sck = 5;
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

#define BLYNK_TEMPLATE_ID "TMPL6lj7rNd6S"
#define BLYNK_TEMPLATE_NAME "Blinking Light"
#define BLYNK_AUTH_TOKEN "Ku47YV9TxJ0faDkYShdTWhCSKmrnllnd"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "No more wifi";
char pass[] = "22044421";

void setup() {
  Serial.begin(57600); 
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  Blynk.begin(auth, ssid, pass);

  LoadCell.begin();
  float calibrationValue = 381.87;
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup is complete");
  }
}

void loop() {
  Blynk.run();

  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;

  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      Blynk.virtualWrite(V0, i); // Assuming V1 is the virtual pin in your Blynk app
      newDataReady = 0;
      t = millis();
    }
  }

  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}
