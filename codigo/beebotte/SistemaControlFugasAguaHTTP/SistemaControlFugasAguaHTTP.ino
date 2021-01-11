#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define SSID "*********"      //SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*********" //Contarseña de dicha red

#define BEEBOTTE_URL_PUT_HUMEDAD "https://api.beebotte.com/v1/data/write/fuga_agua/humedad" //URL para asignar datos al recurso "humedad"
#define BEEBOTTE_URL_GET_INTERRUPTOR "https://api.beebotte.com/v1/data/read/fuga_agua/llave?limit=1&source=raw" //URL para obtener el valor del recurso "llave"
//Se obtiene solicitando la huella de "api.beebotte.com" en la web https://www.grc.com/fingerprints.htm
#define BEEBOTTE_URL_FINGERPRINT "6A:64:AD:21:D9:D5:B6:53:A4:76:2B:6D:CC:A8:FB:1A:53:F7:98:16" //Huella de https://api.beebotte.com
#define TOKEN  "******************" //Token del canal"fuga_agua"

HTTPClient https;
int httpCode;
String payload;

int pinRele = 13;
int humedad;

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);
  
  Serial.begin(115200);

  //Inicializa la conexión WIFI
  Serial.print("Conectando a " + String(SSID) + " ");     
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");
}

void loop() {
  //se lee el nivel de humedad y se mapea en el 0-100
  humedad = analogRead(0);
  humedad = map(humedad, 0, 1024, 100, 0);
  //Cuerpo del mensaje
  String mensajePOST =  "{\"data\":"+String(humedad)+"}";
  Serial.println(mensajePOST);

  //Composición y envío de la petición https de envío de humedad
  https.begin(BEEBOTTE_URL_PUT_HUMEDAD, BEEBOTTE_URL_FINGERPRINT);
  //Cabeceras de la petición https    
  https.addHeader("Content-Type", "application/json");
  https.addHeader("X-Auth-Token", TOKEN);
  //Se envía la petición https con el métofo POST
  httpCode = https.POST(mensajePOST);
  Serial.print("https result: " + String(httpCode));
  payload = https.getString();
  Serial.println(payload);
  https.end(); //se cierra la conexión

  //Composición y envío de la petición https obtención de la posición del interruptor
  https.begin(BEEBOTTE_URL_GET_INTERRUPTOR, BEEBOTTE_URL_FINGERPRINT);
  //Cabeceras de la petición https    
  https.addHeader("Content-Type", "application/json");
  https.addHeader("X-Auth-Token", TOKEN);
  //Se envía la petición https con el método GET
  httpCode = https.GET();
  Serial.print("Resultado http: " + String(httpCode));
  payload = https.getString();
  Serial.println(payload);
  https.end(); //se cierra la conexión

  //Activa o desactiva el relé
  if (payload.indexOf("\"data\":true") >= 0) digitalWrite(pinRele, HIGH);
  else if (payload.indexOf("\"data\":false") >= 0) digitalWrite(pinRele, LOW);

  delay(2000);
}
