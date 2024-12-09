#include "UbidotsESPMQTT.h"
#include <SimpleDHT.h>

#define SSID "*********"      //SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*********" //Contarseña de dicha red

#define TOKEN  "******************"  //Credenciales del API
#define NOMBRE_DISPOSITIVO "wemos-meteorologia" //Nombre del dispositivo
#define VARIABLE_TEMPERATURA "temperatura" //Nombre de la variable con la temperatura
#define VARIABLE_HUMEDAD "humedad" //Nombre de la variable con la humedad

Ubidots client(TOKEN);

//Variables de manejo del sensor DHT11
int pinDHT11 = 12;
SimpleDHT11 dht11;
byte temperatura = 0;
byte humedad = 0;

//Variables para establecer un intervalo de tiempo entre actualizaciones
unsigned int tiempoAnterior;
int intervaloEnviotemperatura = 5000;

void callback(char* topic, byte* payload, unsigned int length) {
}

void setup() {
  Serial.begin(115200);

  //Conexión a la red WIFI
  client.wifiConnection(SSID, PASSWORD);
  //Función de callback MQTT utilizada por Ubidots
  client.begin(callback);
  }

void loop() {
  if(!client.connected()){
      client.reconnect();
      }
  else{
    //Se comprueba si ha pasado el intervalo de tiempo 'intervaloEnviotemperatura'
    if(millis() - tiempoAnterior > intervaloEnviotemperatura){
      dht11.read(pinDHT11, &temperatura, &humedad, NULL); //se lee la temperatura y la humedad
      if (!isnan(temperatura) && !isnan(humedad)){
        Serial.print((int)temperatura); Serial.print(" ºC, ");
        Serial.print((int)humedad); Serial.println(" %");
        // Se publican la humedad y la temperatura
        client.add(VARIABLE_TEMPERATURA, (int)temperatura);
        client.add(VARIABLE_HUMEDAD, (int)humedad);
        client.ubidotsPublish(NOMBRE_DISPOSITIVO);
      }
      tiempoAnterior = millis();
    }
    client.loop();
  }
}
