int pinPulsador = 0;
int pinLed = 2;

void setup(){
 Serial.begin(9600);
 pinMode(pinPulsador, INPUT);
 pinMode(pinLed, OUTPUT);
}
 
void loop(){
 if (Serial.available()) digitalWrite(pinLed, (int)Serial.read());
 else digitalWrite(pinLed, LOW);
 if (digitalRead(pinPulsador)) Serial.print(HIGH);
 delay(10);
}
