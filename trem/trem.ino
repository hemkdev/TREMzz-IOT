// Bibliotecas e Includes
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

const int ledPin = 2;

void setup() {

  Serial.begin(115200);    
  espClient.setInsecure();  
  WiFi.begin(WIFI_SSID, WIFI_PASS);  
  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!");


  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  String userID = "trem";
  userID += String(random(0xffff), HEX);
  while (mqttClient.connect(userID.c_str()) == 0) {
    Serial.print(".");
    delay(200);
  }


  mqttClient.setCallback(callback); 
  mqttClient.subscribe(TOPIC_PONTEH); 
  Serial.println("\n Conectado com sucesso!");


  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}


void loop() {
  String mensagem = "";  
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem = ": " + mensagem;
    mqttClient.publish(TOPIC_STATUS, mensagem.c_str());  
    Serial.println(mensagem);
  }
  mqttClient.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {  
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    
    MensagemRecebida += (char)payload[i];
  }
  Serial.println(MensagemRecebida);


  digitalWrite(2, HIGH);  
  delay(1000);            
  digitalWrite(2, LOW);
}