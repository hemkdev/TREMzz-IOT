#include "env.h"  

#include <WiFi.h>
#include <PubSubClient.h>

// Definições de pinos para LEDs e sensores ultrassônicos
#define LED_PIN 18  

const int PINO_TRIG = 12;  // Pino GPIO 12 conectado ao pino TRIG do primeiro HC-SR04 
const int PINO_ECHO = 13;  // Pino GPIO 13 conectado ao pino ECHO do primeiro HC-SR04 

const int PINO_TRIG2 = 14;  // Pino GPIO 14 conectado ao pino TRIG do segundo HC-SR04
const int PINO_ECHO2 = 15;  // Pino GPIO 15 conectado ao pino ECHO do segundo HC-SR04

const int PINO_R = 25;  
const int PINO_G = 26;  
const int PINO_B = 27;

String currentColor = "NONE";  // Variável para rastrear a cor atual dos LEDs RGB

// WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis globais para sensores e estados

float presence2 = 0;  // Distância medida pelo primeiro sensor
float presence4 = 0;  // Distância medida pelo segundo sensor
bool ledState = false;  // Estado do LED simples (false = desligado, true = ligado)
String ledRgbState = "Desligado";  // Estado dos LEDs RGB
unsigned long lastMsg = 0;
const long interval = 2000;  // Intervalo de 2 segundos para leitura/publicação

// Função para converter valores RGB em string hexadecimal (ex.: 255,0,0 -> "FF0000")
String rgbToHex(int r, int g, int b) {  
  long colorValue = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
  char hexString[7]; 
  snprintf(hexString, sizeof(hexString), "%06lX", colorValue);  // Formata como hex maiúsculo com 6 dígitos
  return String(hexString);  // Retorna a string hexadecimal
}

// Função para definir cor dos LEDs RGB usando valores RGB (0-255)
void setColor(int redValue, int greenValue, int blueValue) {
  currentColor = rgbToHex(redValue, greenValue, blueValue);  // Atualiza a variável de cor atual
  analogWrite(PINO_R, redValue);  
  analogWrite(PINO_G, greenValue);  
  analogWrite(PINO_B, blueValue);
}

// Função para definir cor dos LEDs RGB usando string hexadecimal

void setColor(String hex) {
  if (hex.startsWith("#")) { 
  }
  long colorValue = strtol(hex.c_str(), NULL, 16);  
  int r = (colorValue >> 16) & 0xFF;  
  int g = (colorValue >> 8) & 0xFF;   
  int b = colorValue & 0xFF;   
  analogWrite(PINO_R, r);  
  analogWrite(PINO_G, g); 
  analogWrite(PINO_B, b); 
}

// Função para configurar e conectar à WiFi

void setup_wifi() {
  delay(10);  // delay para estabilização
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_PASS);
  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  while (WiFi.status() != WL_CONNECTED) {  
    delay(500);  
    Serial.print("."); 
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");  
  Serial.println(WiFi.localIP()); 
}

// Função callback para mensagens MQTT recebidas

void callback(char* topic, byte* payload, unsigned int length) {
  digitalWrite(LED_PIN, !ledState);  // Inverte o estado do LED simples
  ledState = !ledState;  // Atualiza a variável de estado
}

// Função para reconectar ao broker MQTT se desconectado
void reconnect() {
  while (!client.connected()) {  // Loop até conectar
    Serial.print("Attempting MQTT connection...");  
    String clientId = "ESP32_S2_";  
    clientId += String(random(0xffff), HEX);  
    if (client.connect(clientId.c_str(), BROKER_USER_ID, BROKER_USER_PASS)) {  
      Serial.println("connected");  
      client.subscribe(TOPIC_LED);  
    } else {
      Serial.print("failed, rc=");  
      Serial.print(client.state());  
      Serial.println(" try again in 5 seconds");  
      delay(5000);  
    }
  }
}

// Função para ler distâncias dos sensores ultrassônicos HC-SR04
void readSensors() {
  digitalWrite(PINO_TRIG, LOW);  
  delayMicroseconds(2);  
  digitalWrite(PINO_TRIG, HIGH);  
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);  
  long duracao1 = pulseIn(PINO_ECHO, HIGH);  
  presence2 = (duracao1 * 0.0343) / 2;  
  
  delay(20);
  
  digitalWrite(PINO_TRIG2, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG2, LOW);
  long duracao2 = pulseIn(PINO_ECHO2, HIGH);
  presence4 = (duracao2 * 0.0343) / 2;
}

// Função para publicar dados dos sensores via MQTT
void publishData() {
  client.publish(TOPIC_PRESENCA2, String(presence2).c_str());
  client.publish(TOPIC_PRESENCA4, String(presence4).c_str()); 
}

// Função setup: executada uma vez no início
void setup() {
  Serial.begin(115200);  

  // Configura pinos como entrada/saída
  pinMode(LED_PIN, OUTPUT); 
  pinMode(PINO_TRIG, OUTPUT);  
  pinMode(PINO_ECHO, INPUT);   
  pinMode(PINO_TRIG2, OUTPUT); 
  pinMode(PINO_ECHO2, INPUT);  
  pinMode(PINO_R, OUTPUT);     
  pinMode(PINO_G, OUTPUT);    
  pinMode(PINO_B, OUTPUT);     
  
  // Desliga todos os LEDs inicialmente
  digitalWrite(LED_PIN, LOW);
  digitalWrite(PINO_R, LOW);
  digitalWrite(PINO_G, LOW);
  digitalWrite(PINO_B, LOW);
  
  // Conecta à WiFi
  setup_wifi();
  
  // Configura MQTT: servidor, porta e callback
  client.setServer(BROKER_URL, BROKER_PORT);
  client.setCallback(callback);
  
  Serial.println("S2 Station initialized!");  // Confirmação de inicialização
}

// Função loop: executada repetidamente
void loop() {
  if (!client.connected()) {  // Se MQTT desconectado, reconecta
    reconnect();
  }
  client.loop();  // Processa mensagens MQTT recebidas
  
  unsigned long now = millis();
  if (now - lastMsg > interval) {  
    lastMsg = now;  
    readSensors();  
    publishData();  
  }
}
