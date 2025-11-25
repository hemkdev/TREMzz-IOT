// Bibliotecas e Includes
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

// Inicialização de objetos
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// Pinos
const int ledPin = 2;

// Setup
void setup() {

// Conexão WiFi
  Serial.begin(115200);    // inicia a Serial para ver mensagens de debug
  espClient.setInsecure();  // desativa verificação do certificado (somente para testes)
  WiFi.begin(WIFI_SSID, WIFI_PASS);  // inicia conexão WiFi
  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!");

// Conexão MQTT
  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  String userID = "trem";
  userID += String(random(0xffff), HEX);
  while (mqttClient.connect(userID.c_str()) == 0) {
    Serial.print(".");
    delay(200);
  }

// Inscrição e Callback
  mqttClient.setCallback(callback); // define a função chamada quando chegar mensagem no tópico
  mqttClient.subscribe(TOPIC_PONTEH); // inscreve no tópico configurado
  Serial.println("\n Conectado com sucesso!");

// Configuração de pinos
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

// Loop principal
void loop() {
  String mensagem = "";  // lê letra por letra e adiciona \n automático no final
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem = ": " + mensagem;
    mqttClient.publish(TOPIC_STATUS, mensagem.c_str());  // Envia a mensagem pro servidor Tópico
    Serial.println(mensagem);
  }
  mqttClient.loop();
}

// Função Callback - chamada ao receber mensagem MQTT
void callback(char* topic, byte* payload, unsigned int length) {  // Tópico, mensagem em si, tamanho da mensagem
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    // Pega cada letra de payload e junta na mensagem
    MensagemRecebida += (char)payload[i];
  }
  Serial.println(MensagemRecebida);

// Código para acender ou apagar o led (pino 2)
  digitalWrite(2, HIGH);  // Pisca ao receber mensagem
  delay(1000);            
  digitalWrite(2, LOW);
}