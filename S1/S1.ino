#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);


const String brokerUser = "";
const String BrokerPass = "";

const int ledPin = 2;

// estado do piscar (não bloqueante)
volatile bool blinkRequested = false;
unsigned long blinkUntil = 0;

void setup() {
  Serial.begin(115200);    // inicia a Serial para ver mensagens de debug
  espClient.setInsecure();  // desativa verificação do certificado (somente para testes)
  WiFi.begin(WIFI_SSID, WIFI_PASS);  // inicia conexão WiFi
  Serial.println("Conectando no WiFi");
  Serial.println("Conectado com sucesso");
    mqttClient.setServer(brokerURL, brokerPort);
  String userID = "S1";
  userID += String(random(0xffff), HEX);
  while (!mqttClient.connect(userID.c_str(), BROKER_USER_ID, BROKER_USER_PASS)) {
    Serial.print(".");
    delay(200);
  }
  mqttClient.setCallback(callback); // define a função chamada quando chegar mensagem no tópico
  mqttClient.subscribe(topicPresence); // inscreve no tópico configurado
  Serial.println("\n Conectado com sucesso!");
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  String mensagem = "";  // lê uma linha do Serial (até '\\n')
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
  mqttClient.publish(topicPresence, mensagem.c_str());  // publica a mensagem no tópico MQTT
    Serial.println(mensagem);
  }
  mqttClient.loop();
  // trata o piscar do LED sem bloquear o programa
  unsigned long now = millis();
  if (blinkRequested) {
    if (now <= blinkUntil) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
      blinkRequested = false;
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {  // tópico, payload e tamanho
  // monta uma string C a partir do payload (evita usar String no callback)
  const unsigned int maxLen = 255;
  unsigned int copyLen = (length < maxLen) ? length : maxLen;
  char msg[maxLen + 1];
  for (unsigned int i = 0; i < copyLen; i++) {
    msg[i] = (char)payload[i];
  }
  msg[copyLen] = '\0';

  Serial.print("Mensagem recebida no tópico ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);

  // pede para piscar o LED por 1s (feito no loop, sem bloquear)
  blinkRequested = true;
  blinkUntil = millis() + 1000;
}

