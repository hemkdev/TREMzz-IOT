#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;
const String topic = "hemk";

const String brokerUser = "";
const String BrokerPass = "";

const int ledPin = 2;

void setup() {
  Serial.begin(115200);    //configura a placa pra mostrar na tela
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  Serial.println("Conectando no WiFi");
  Serial.println("Conectado com sucesso");
  mqttClient.setServer(brokerURL.c_str(), brokerPort);
  String userID = "estação_principal";
  userID += String(random(0xffff), HEX);
  while (mqttClient.connect(userID.c_str()) == 0) {
    Serial.print(".");
    delay(200);
  }
  mqttClient.subscribe(topic.c_str());
  mqttClient.setCallback(callback);  // Função ao receber mensagem no tópico
  Serial.println("\n Conectado com sucesso!");
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  String mensagem = "";  // lê letra por letra e adiciona \n automático no final
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem = "Hemk: " + mensagem;
    mqttClient.publish("bia_santos", mensagem.c_str());  // Envia a mensagem pro servidor Tópico
    Serial.println(mensagem);
  }
  mqttClient.loop();
}

void callback(char* topic, byte* payload, unsigned long length) {  // Tópico, mensagem em si, tamanho da mensagem
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
