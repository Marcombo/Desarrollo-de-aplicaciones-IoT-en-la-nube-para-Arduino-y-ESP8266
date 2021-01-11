#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // Contraseña de dicha red

const char* mqtt_server = "mqtt.beebotte.com"; //broker MQTT de Beebotte
const char* tema_sensor_humedad = "fuga_agua/humedad"; //tema en el que publica el sensor
const char* tema_llave_agua = "fuga_agua/llave"; //tema al que se suscribe para recibir órdenes del interruptor Beebotte
const char usuario[] = "******************"; //Token del canal fugas de agua

WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

int pinRele = 13;
int humedad, humedadAnterior;

//Variables para establecer un intervalo de tiempo entre actualizaciones
unsigned int tiempoAnterior;
int intervaloEnviotemperatura = 2000;

//Función que se ejecuta cuando se recibe un mensaje de alguno de los temas a los que está suscrito
//Solo está suscrito al que rcibe las órdenes de encendido/apagado del relé desde Beebotte
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
  if (mensaje.indexOf("\"data\":true") >= 0) digitalWrite(pinRele, HIGH);
  else if (mensaje.indexOf("\"data\":false") >= 0) digitalWrite(pinRele, LOW);
}

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);
  
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
  //Establecemos la función a la que se va a llamar cuando llegue un mensaje del broker
  clienteMQTT.setCallback(callback);
  }

void loop() {  
  //Establecemos la conexión con el broker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos", usuario, "")) { 
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema_llave_agua);
      } else {
        delay(5000);
      }
  }
  if(millis() - tiempoAnterior > intervaloEnviotemperatura){
    //se lee el nivel de humedad y se mapea en el 0-100
    humedad = analogRead(0);
    humedad = map(humedad, 0, 1024, 100, 0);
    if(humedad != humedadAnterior){
      char datoJSON[100];
      int tmp = sprintf(datoJSON, "{\"data\":%i,\"write\": true}", humedad);
      clienteMQTT.publish(tema_sensor_humedad, datoJSON);

      humedadAnterior = humedad;
    }
    
    tiempoAnterior = millis();
  }
  
  // Cliente escuchando
  clienteMQTT.loop();
}
