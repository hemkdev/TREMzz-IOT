#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifi_client;
PubSubClient mqtt(wifi_client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;

const String brokerUser = "";
const String BrokerPass = "";

void setup() {
  Serial.begin(115200);    //configura a placa pra mostrar na tela
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(2000);
  }
  Serial.println("Conectado com sucesso");
  mqtt.setServer(brokerURL.c_str(),brokerPort);
  String clientID = "estação_principal";
  clientID += String(random(0xffff), HEX);
  while(mqtt.connect(clientID.c_str()) == 0 ){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\n Conectado ao broker!");
}

void loop() {
}