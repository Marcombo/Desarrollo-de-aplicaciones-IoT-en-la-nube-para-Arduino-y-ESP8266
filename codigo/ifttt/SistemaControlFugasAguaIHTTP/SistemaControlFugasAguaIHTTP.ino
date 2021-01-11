#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define SSID "*********"      //SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*********" //Contarseña de dicha red

#define IFTTT_URL_WEBHOOK "https://maker.ifttt.com/trigger/fuga_agua/with/key/*********" //URL del webhook IFTTT
//Se obtiene solicitando la huella de "api.beebotte.com" en la web https://www.grc.com/fingerprints.htm
#define IFTTT_URL_FINGERPRINT "AA:75:CB:41:2E:D5:F9:97:FF:5D:A0:8B:7D:AC:12:21:08:4B:00:8C" //Huella de https://maker.ifttt.com

HTTPClient https;
int httpCode;
String payload;

int humedad;
boolean notificacionEnviada = false;

void setup() {
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
  if(humedad > 5){
    if(!notificacionEnviada){
      char UrlWebhookGet[100];
      int tmp = sprintf(UrlWebhookGet, "%s?value1=%i", IFTTT_URL_WEBHOOK, humedad);

      https.begin(UrlWebhookGet, IFTTT_URL_FINGERPRINT);
      //Se envía la petición https con el método GET
      httpCode = https.GET(); 
      Serial.print("Resultado http: " + String(httpCode));
      payload = https.getString();
      Serial.println(payload);
      https.end(); //se cierra la conexión
 
      notificacionEnviada = true;
    }
  }
  else notificacionEnviada = false;
  delay(2000);
}
