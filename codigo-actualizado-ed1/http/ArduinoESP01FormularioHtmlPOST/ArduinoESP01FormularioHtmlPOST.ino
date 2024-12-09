#include <SoftwareSerial.h>

SoftwareSerial puertoESP01(12, 13); // RX | TX

char caracter;
String orden;
 
void setup() {
   Serial.begin(115200);
   puertoESP01.begin(9600);

   pinMode(3, OUTPUT);
   pinMode(5, OUTPUT);
   pinMode(6, OUTPUT);
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(11, OUTPUT);
}
 
void loop() {
  orden = "";
  while(puertoESP01.available() > 0){
    caracter = puertoESP01.read(); 
    orden = orden + caracter;
  }
  if (orden != ""){
    int gpio = orden.substring(orden.indexOf("GPIO=")+5, (orden.indexOf("&"))).toInt();
    int valor = orden.substring(orden.indexOf("Valor=")+6).toInt();
    Serial.println("GPIO: " + String(gpio) + " Valor: " + String(valor));
    
    analogWrite(gpio, valor);
  }
}

