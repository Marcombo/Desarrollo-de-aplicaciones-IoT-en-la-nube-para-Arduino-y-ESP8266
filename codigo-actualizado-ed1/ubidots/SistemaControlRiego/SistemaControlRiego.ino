#include "UbidotsESPMQTT.h"

#define SSID "*********"      // SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*********" //Contraseña de dicha red

#define TOKEN  "******************"  //Credenciales del API
#define NOMBRE_DISPOSITIVO "wemos-riego" //Nombre del dispositivo
#define VARIABLE_HUMEDAD "humedad" //Nombre de la variable con la humedad
#define VARIABLE_RELE "rele" //variable modificada por el interruptor de Ubidots

Ubidots client(TOKEN);

int pinRele = 13;
int humedad;

//Variables para establecer un intervalo de tiempo entre actualizaciones
unsigned int tiempoAnterior;
int intervaloEnviotemperatura = 5000;

//Función que se ejecuta cuando se recibe unmensaje de alguno de los temas a los que está suscrito
void callback(char* tema, byte* contenido, unsigned int longitudContenido) {
  String mensaje = "";
  Serial.print("Ha llegado un mensaje con el tema: ");
  Serial.print(tema);
  Serial.print(" y el contenido: ");
  for (int i = 0; i < longitudContenido; i++) {
    mensaje = mensaje + (char)contenido[i];
    Serial.print((char)contenido[i]);
  }
  Serial.println();
  //Activa o desactiva el relé
  if (mensaje == "1") digitalWrite(pinRele, HIGH);
  else if (mensaje == "0") digitalWrite(pinRele, LOW); 
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
  else{
    //Se comprueba si ha pasado el intervalo de tiempo 'intervaloEnviotemperatura'
    if(millis() - tiempoAnterior > intervaloEnviotemperatura){
      //se lee el nivel de humedad y se mapea en el 0-100
      humedad = analogRead(0);
      humedad = map(humedad, 0, 1024, 100, 0);

      // Se publica la humedad
      client.add(VARIABLE_HUMEDAD, (int)humedad);
      client.ubidotsPublish(NOMBRE_DISPOSITIVO);

      tiempoAnterior = millis();
    }
    client.loop();
  }
}
