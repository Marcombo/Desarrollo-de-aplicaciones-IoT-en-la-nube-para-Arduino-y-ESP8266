#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // contraseña de dicha red

const char* mqtt_server = "test.mosquitto.org";
const char* tema = "marcombo_temperatura";
WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

int pinDHT11 = 12;
SimpleDHT11 dht11;
byte temperatura = 0;
byte humedad = 0;

unsigned int tiempoAnterior;
int intervaloEnviotemperatura = 2000;

void setup() {
  Serial.begin(115200);

  pinMode(pinDHT11, INPUT);

  //Inicia la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Establece el bróker que se utilizará
  clienteMQTT.setServer(mqtt_server, 1883);
}

void loop() {
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al bróker ...");
      if (clienteMQTT.connect ("wemos", "marcombo_temperatura", 1, true, "Error")) {
        Serial.println("Conectado al bróker.");
      } else {
        delay(5000);
      }
  }
  if(millis() - tiempoAnterior > intervaloEnviotemperatura){
    dht11.read(pinDHT11, &temperatura, &humedad, NULL);
    if (!isnan(temperatura)){
      Serial.print((int)temperatura); Serial.println(" ºC, ");
      char tmp[4];
      dtostrf((int)temperatura,0, 0, tmp);
      clienteMQTT.publish(tema, tmp, true);
    }
    tiempoAnterior = millis();
  }
  // Cliente escuchando
  clienteMQTT.loop();
}

