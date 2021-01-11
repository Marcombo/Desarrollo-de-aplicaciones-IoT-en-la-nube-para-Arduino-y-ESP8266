#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // Contraseña de dicha red

const char* mqtt_server = "things.ubidots.com"; //broker MQTT de Ubidots
const char* tema_estacion_meteorologica = "/v1.6/devices/wemos-meteorologia"; //tema en el que publica la humedad y la temperatura
const char usuario[] = "******************"; //Token del API

WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

//Variables de manejo del sensor DHT11
int pinDHT11 = 12;
SimpleDHT11 dht11;
byte temperatura = 0;
byte humedad = 0;

//Variables para establecer un intervalo de tiempo entre actualizaciones
unsigned int tiempoAnterior;
int intervaloEnviotemperatura = 5000;

void setup() {  
  Serial.begin(115200);

  //Inicializamos la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Establecemos el broker que utilizaremos
  clienteMQTT.setServer(mqtt_server, 1883);
  }

void loop() {  
  //Establecemos la conexión con el broker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos", usuario, "")) { 
        Serial.println("Conectado al broker.");
      } else {
        delay(5000);
      }
  }
  
  if(millis() - tiempoAnterior > intervaloEnviotemperatura){
      dht11.read(pinDHT11, &temperatura, &humedad, NULL); //se lee la temperatura y la humedad
      if (!isnan(temperatura) && !isnan(humedad)){
        Serial.print((int)temperatura); Serial.print(" ºC, ");
        Serial.print((int)humedad); Serial.println(" %");
        // Se publican la humedad y la temperatura
        char mensaje[100];
        int tmp = sprintf(mensaje, "{\"humedad\":%i,\"temperatura\":%i}", humedad, temperatura);
        Serial.println(mensaje);
        clienteMQTT.publish(tema_estacion_meteorologica, mensaje);
      }
      tiempoAnterior = millis();
    }
  
  // Cliente escuchando
  clienteMQTT.loop();
}
