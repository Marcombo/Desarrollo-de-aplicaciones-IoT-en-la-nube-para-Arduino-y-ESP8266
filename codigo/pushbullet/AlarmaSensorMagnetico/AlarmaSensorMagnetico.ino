#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // contraseña de dicha red

#define PUSHBULLET_API_URL "https://api.pushbullet.com/v2/pushes"  //URL del API
//Se obtiene poniendo "https://api.pushbullet.com" en la web https://www.grc.com/fingerprints.htm
#define PUSHBULLET_API_URL_FINGERPRINT "FA:C0:BE:4E:64:B8:80:C7:B7:EC:B6:D9:01:FF:61:0D:AC:92:F5:A6" 
#define PUSHBULLET_API_KEY "******************"  //Token de acceso

int pinSensorMagnetico = 13;
boolean sensorActivado = false;
boolean alarmaRearmada = true;

void setup() {
  Serial.begin(115200);
  pinMode(pinSensorMagnetico, INPUT_PULLUP);

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
  sensorActivado = digitalRead(pinSensorMagnetico);
  if(!sensorActivado) alarmaRearmada = true;
  if(sensorActivado && alarmaRearmada){
    Serial.println("Ha saltado la alarma");

    HTTPClient http;
    //Composición y envío de la petición HTTPS
    http.begin(PUSHBULLET_API_URL , PUSHBULLET_API_URL_FINGERPRINT);
    //Cabeceras de la petición HTTP  
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Access-Token", PUSHBULLET_API_KEY);
    String postMessage =  "{\"type\":\"note\",\"title\":\"Alarma\",\"body\":\"Alguien ha entrado en casa\"}";
    //Se envía la petición HTTP con el métofo POST
    int httpCode = http.POST(postMessage); //Se envía el emensaje
    Serial.print("Resultado http: ");
    Serial.println(httpCode);
    String payload = http.getString();
    Serial.println(payload);
    http.end(); //se cierra la conexión

    alarmaRearmada = false;
  }
  delay(100);
}
