
#include <WiFi.h>

#include <WiFiClientSecure.h>

#include <PubSubClient.h>

WiFiClientSecure client;

PubSubClient mqtt(client);


const String SSID = "FIESC_IOT_EDU";

const String PASS = "8120gv08";

const int PORT = 8883; 

const String URL = "066fc2635b7c4272a28d66b64f3a555a.s1.eu.hivemq.cloud";


const String broker_user = "gabespancado"; 

const String broker_pass = "Guisoco123";


const int ledPin1 = 9; 

const int ledPin2 = 17;


void setup() {
 
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  
  client.setInsecure();
  
  Serial.begin(115200);
  
  Serial.println("Conectando ao Wifi"); 
 
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  
  Serial.println("\nConectado!");
  
  Serial.println("Conectando ao broker...");
 
  mqtt.setServer(URL.c_str(), PORT);
  
  while (!mqtt.connected()) { 

    String ID = "TREM-";
    ID += String(random(0xffff), HEX);
    mqtt.connect(ID.c_str(), broker_user.c_str(), broker_pass.c_str());
    delay(200);
    Serial.print(".");
  }
  
  mqtt.subscribe("trem"); 
  
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker com sucesso!");
}


void loop() { 

  mqtt.loop();
  delay(10);
}

void callback(char* topic, byte* payload, unsigned int length) { 
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Recebido:  ");
  Serial.println(mensagem);
  
  int vel = mensagem.toInt();
  
  if (vel < 0) { 
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
  }
  
  else if (vel > 0) { 
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, HIGH);
  }
  
  else { 
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
  }
}
