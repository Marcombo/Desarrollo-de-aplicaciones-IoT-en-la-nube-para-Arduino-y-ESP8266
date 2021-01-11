#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // Contraseña de dicha red

const char* mqtt_server = "mqtt.beebotte.com"; //broker MQTT de Beebotte
const char* tema_led_rojo = "control_leds/rojo"; //tema en el que publica los mensajes del led rojo
const char* tema_led_verde = "control_leds/verde"; //tema en el que publica los mensajes del led verde
const char usuario[] = "*******************"; //Token del canal control_leds

WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

int pinLedRojo = 13;
int pinLedVerde = 12;

//Función que se ejecuta cuando se recibe un mensaje de alguno de los temas a los que está suscrito
//Está suscrito a los que recibe los mensajes que representan el led rojo ("rojo") y el led verde ("verde") en Beebotte
void callback(char* tema, byte* contenido, unsigned int longitudContenido) {
  String mensaje;
  int pinLed;
  Serial.print("Ha llegado un mensaje con el tema: ");
  Serial.print(tema);
  Serial.print(" y el contenido: ");
  for (int i = 0; i < longitudContenido; i++) {
    mensaje = mensaje + (char)contenido[i];
    Serial.print((char)contenido[i]);
  }
  Serial.println();
  
  if(String(tema) == "control_leds/rojo") pinLed = pinLedRojo;
  else pinLed = pinLedVerde;
  if(mensaje.indexOf("\"data\":\"ON\"") >= 0) digitalWrite(pinLed, HIGH);
  else digitalWrite(pinLed, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(pinLedRojo, OUTPUT);
  digitalWrite(pinLedRojo, LOW);
  pinMode(pinLedVerde, OUTPUT);
  digitalWrite(pinLedVerde, LOW);

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
  //Establecemos la función a la que se va a llamar cuando llegue un mensaje del broker
  clienteMQTT.setCallback(callback);
}

void loop() {  
  //Establecemos la conexión con el broker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos", usuario, "")) { 
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema_led_rojo);
        clienteMQTT.subscribe(tema_led_verde);
      } else {
        delay(5000);
      }
  }  
  // Cliente escuchando
  clienteMQTT.loop();
}
