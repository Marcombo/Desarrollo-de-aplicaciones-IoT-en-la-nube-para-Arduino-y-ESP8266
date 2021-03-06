#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // contraseña de dicha red

const char* mqtt_server = "test.mosquitto.org";
const char* tema = "marcombo_pir";
WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

int pinSirena = 13;
int pinPulsador = 12;

volatile boolean sirenaActivada = false;

//Función que atiende la interrupción generada por el pulsador
void ICACHE_RAM_ATTR atenderInterrupcion() {
  sirenaActivada = false;
}

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
  //Se activa el relé
  if (mensaje == "ON") sirenaActivada = true;
}
 
void setup() {
  Serial.begin(115200);
  pinMode(pinSirena, OUTPUT);
  pinMode(pinPulsador, INPUT);

  //Inicia la conexión WIFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Asigna el bróker que se utiliza
  clienteMQTT.setServer(mqtt_server, 1883);
  //Indica la función a la que se va a llamar cuando llegue un mensaje del bróker
  clienteMQTT.setCallback(callback);

  //Asocia la función que atenderá la interrupción generada al presionar el pulsador
  attachInterrupt(digitalPinToInterrupt(pinPulsador), atenderInterrupcion, RISING);
}
 
void loop(){
  //Si ha presionado el pulsador y suena la sirena
  if(sirenaActivada){
    for(int frecuencia = 400; frecuencia < 1000; frecuencia+=25){
    tone(pinSirena, frecuencia, 50);
    delay(5);
    }
    for(int frecuencia = 1000; frecuencia > 400; frecuencia-=25){
      tone(pinSirena, frecuencia, 50);
      delay(5);
    }
  }

  //Establece la conexión con el bróker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemosRele")) {
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema);
      } else {
        delay(5000);
      }
  }
  //Conexión establecida
  clienteMQTT.loop();
}

