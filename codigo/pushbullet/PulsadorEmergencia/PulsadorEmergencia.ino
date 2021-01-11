#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // contraseña de dicha red

#define PUSHBULLET_API_URL "https://api.pushbullet.com/v2/pushes"  //URL del API
//Se obtiene poniendo "https://api.pushbullet.com" en la web https://www.grc.com/fingerprints.htm
#define PUSHBULLET_API_URL_FINGERPRINT "FA:C0:BE:4E:64:B8:80:C7:B7:EC:B6:D9:01:FF:61:0D:AC:92:F5:A6" 
#define PUSHBULLET_API_KEY "******************"  //Token de acceso

int pinPulsador = 13;
int pinBuzzer = 12;
boolean estadoPulsador = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinPulsador, INPUT);
  pinMode(pinBuzzer, OUTPUT);

  //Inicializa la conexión WIFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");
}

void loop() {
  estadoPulsador = digitalRead(pinPulsador);
  if(estadoPulsador){
    Serial.println("Se ha pulsado el botón de emergencia");

    HTTPClient http;
    //Composición y envío de la petición HTTPS
    http.begin(PUSHBULLET_API_URL , PUSHBULLET_API_URL_FINGERPRINT);
    //Cabeceras de la petición HTTP  
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Access-Token", PUSHBULLET_API_KEY);
    String postMessage =  "{\"type\":\"note\",\"title\":\"Alarma\",\"body\":\"Se ha pulsado el botón de emergencia\"}";
    //Se envía la petición HTTP con el métofo POST
    int httpCode = http.POST(postMessage); //Se envía el emensaje
    Serial.print("Resultado http: ");
    Serial.println(httpCode);
    String payload = http.getString();
    Serial.println(payload);
    http.end(); //se cierra la conexión

    while (digitalRead(pinPulsador)){
      analogWrite(pinBuzzer, 20);
      delay(100);
    }
    analogWrite(pinBuzzer, 0);
  }
}
