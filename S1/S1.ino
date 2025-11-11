#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

const String brokerUser = "";
const String BrokerPass = "";

const int ledPin = pino_led; // usa o pino definido em env.h

// estado do piscar (não bloqueante)
volatile bool blinkRequested = false;
unsigned long blinkUntil = 0;

void callback(char* topic, byte* payload, unsigned int length); // protótipo

void reconnect() {
  // tenta reconectar ao broker MQTT
  while (!mqttClient.connected()) {
    Serial.print("Tentando conectar MQTT...");
    String clientId = String(DEVICE_NAME);
    clientId += "-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str(), BROKER_USER_ID, BROKER_USER_PASS)) {
      Serial.println("conectado");
      mqttClient.setCallback(callback);

      mqttClient.subscribe(topicPresence);
      mqttClient.subscribe("#");//todos os tópicos
    } else {
      Serial.print("falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando novamente em 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  espClient.setInsecure(); // desativa verificação do certificado (somente para testes)

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(brokerURL, brokerPort);
  reconnect();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  // Pinos - Ultrassônico
  pinMode(pino_ultraTrig, OUTPUT);
  pinMode(pino_ultraEcho, INPUT);
  
  // Inicializar o pino trigger em LOW
  digitalWrite(pino_ultraTrig, LOW);
  
  // Debug dos pinos configurados
  Serial.println("=== Configuração do Sensor Ultrassônico ===");
  Serial.print("Pino Trigger: ");
  Serial.println(pino_ultraTrig);
  Serial.print("Pino Echo: ");
  Serial.println(pino_ultraEcho);
  Serial.println("Sensor configurado!");
}

float lerDistancia() {
  // Debug dos pinos configurados
  Serial.print("Testando sensor - Trigger: ");
  Serial.print(pino_ultraTrig);
  Serial.print(", Echo: ");
  Serial.println(pino_ultraEcho);
  
  // Garantir que o trigger está LOW por mais tempo
  digitalWrite(pino_ultraTrig, LOW);
  delayMicroseconds(5);
  
  // Enviar pulso de trigger de 10us
  digitalWrite(pino_ultraTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pino_ultraTrig, LOW);
  
  // Ler duração do pulso no pino echo com timeout maior
  long duracao = pulseIn(pino_ultraEcho, HIGH, 50000); // timeout de 50ms
  
  // Debug da duração lida
  Serial.print("Duração do pulso: ");
  Serial.print(duracao);
  Serial.println(" microsegundos");
  
  // Verificar se houve timeout
  if (duracao == 0) {
    Serial.println("Erro: timeout - sensor não responde ou mal conectado");
    
    // Teste adicional - verificar se o pino Echo está funcionando
    Serial.print("Estado atual do pino Echo: ");
    Serial.println(digitalRead(pino_ultraEcho));
    
    return -1; // indica erro
  }
  
  // Verificar se a duração está em um range válido (2cm a 400cm)
  if (duracao < 116 || duracao > 23200) { // ~2cm a ~400cm
    Serial.print("Duração fora do range válido: ");
    Serial.println(duracao);
    return -1;
  }
  
  // Calcular distância: velocidade do som = 343m/s = 0.0343cm/us
  // Tempo de ida e volta, então dividir por 2
  float distancia = (duracao * 0.0343) / 2;
  
  Serial.print("Distância calculada: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  return distancia;
}

void loop() {
  // reconecta se necessário
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // publica qualquer texto enviado pela Serial no tópico de presença
  if (Serial.available() > 0) {
    String mensagem = Serial.readStringUntil('\n');
    mqttClient.publish(topicPresence, mensagem.c_str());
    Serial.println("Mensagem publicada: "+mensagem);
  }

  //ler dist
  //if < 10 -> publica alguma msg
  float distancia = lerDistancia();
  
  if (distancia >= 0) { // se não houve erro
    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.println(" cm");
    
    if (distancia < 20) {
      mqttClient.publish(topicPresence, "Presença detectada");
      Serial.println("Presença detectada!");
    }
  } else {
    Serial.println("Erro na leitura do sensor");
  }

  //ler temp -> publica valor
  //ler umid -> publica valor
  
  //ler iluminação
  //if < X -> publica acender
  //else -> publica apagar

  delay(2000);


}

void callback(char* topic, byte* payload, unsigned int length) {
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

  // comportamento padrão: piscar LED por 1s ao receber qualquer mensagem
  blinkRequested = true;
  blinkUntil = millis() + 1000;

  // se for comando para o tópico de controle do LED, processa ON/OFF
  if (strcmp(topic, topicLed) == 0) {
    if (strcasecmp(msg, "ON") == 0 || strcmp(msg, "1") == 0) {
      digitalWrite(ledPin, HIGH);
    } else if (strcasecmp(msg, "OFF") == 0 || strcmp(msg, "0") == 0) {
      digitalWrite(ledPin, LOW);
    }
  }
}

