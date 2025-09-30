#include <DHT.h>

#define DHTPIN 1
#define DHTTYPE DHT11

DHT sensorTempUmd(DHTPIN, DHTTYPE);

float umidade = 0;
float temperatura = 0;

void setup() {
  Serial.begin(9600);
  sensorTempUmd.begin();


}

void loop() {
  umidade = sensorTempUmd.readTemperature();
  temperatura = sensorTempUmd.readTemperature();

  Serial.print(umidade);
  Serial.print(" ");
  Serial.println(umidade);
  delay(2000);


}