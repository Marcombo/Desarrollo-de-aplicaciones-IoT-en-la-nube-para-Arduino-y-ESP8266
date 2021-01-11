#include "UbidotsESPMQTT.h"

#define SSID "*********"      // SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*********" //Contraseña de dicha red

#define TOKEN  "*************"  //Credenciales del API
#define NOMBRE_DISPOSITIVO "wemos-garaje" //Nombre del dispositivo
#define VARIABLE_RELE "puerta" //variable modificada por IFTTT

Ubidots client(TOKEN);

int pinRele = 13;

//Función que se ejecuta cuando se recibe unmensaje de alguno de los temas a los que está suscrito
void callback(char* tema, byte* contenido, unsigned int longitudContenido) {
  //Activa el relé durante un segundo
  digitalWrite(pinRele, HIGH);
  delay(1000);
  digitalWrite(pinRele, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);
  
  client.ubidotsSetBroker("business.api.ubidots.com"); // Asignamos el broker a utilizar
  //Conexión a la red WIFI
  client.wifiConnection(SSID, PASSWORD);
  //Función de callback
  client.begin(callback);
  //Se suscribe al cambio de valores de la variable VARIABLE_LABEL
  client.ubidotsSubscribe(NOMBRE_DISPOSITIVO, VARIABLE_RELE);
  }

void loop() {
  if(!client.connected()){
      client.reconnect();
      //Se vuelve a suscribir en caso de haber perdido la conexión
      client.ubidotsSubscribe(NOMBRE_DISPOSITIVO, VARIABLE_RELE); 
  }
  client.loop();
}
