const byte echo_pin = 35;
const byte trigg_pin = 32;
int distancia = 0;
unsigned long tempo = 0;


void setup() {
  pinMode(echo_pin, INPUT);
  pinMode(trigg_pin, OUTPUT);
  Serial.begin(9600);



}

void loop() {
 digitalWrite(trigg_pin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigg_pin, LOW); 
 tempo = pulseIn(echo_pin, HIGH);
 distancia = ((tempo *340.29)/2)/10000;
 if (distancia > 100) {
  Serial.println("Longe"); 
 } else if (distancia <= 100 && distancia > 50) {
  Serial.println("Média");
 } else {
  Serial.println("Perto");
 }
 delay(500);
}