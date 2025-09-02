const byte ldr_pin = 32;
int luz= 0; 

void setup() {
  pinMode(ldr_pin, INPUT);
  Serial.begin(9600);

}

void loop() {
  luz = analogRead(ldr_pin);
  Serial.println(luz);
  delay(100);

}