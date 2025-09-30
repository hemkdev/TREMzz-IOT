#include <WiFi.h>
WiFiClient client;
const String SSID = "Nome da rede";
const String PASS = "Senha da rede";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando no WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(2000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
