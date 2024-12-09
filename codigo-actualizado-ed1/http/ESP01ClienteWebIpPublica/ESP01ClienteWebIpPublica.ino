#include <ESP8266WiFi.h>                        
 
const char* ssid = "*************";                   // SSID de la red WIFI a la que desea conectarse
const char* password = "*******************";        //contrseña de dicha red

const char* servidorWeb = "www.vermiip.es";            
 
void setup()
{
  Serial.begin(115200);

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
 
void loop()
{
  WiFiClient clienteWeb;                                 

  //Establece la conexión con el servidor  
  if (clienteWeb.connect(servidorWeb, 80))
  {
    //Realiza la petición de la página HTML
    clienteWeb.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + servidorWeb + "\r\n" +
                 "Connection: close\r\n" + "\n");
 
    //Lee la respuesta del servidor linea a linea
    while (clienteWeb.connected())
    {
      if (clienteWeb.available())
      {
        String linea = clienteWeb.readStringUntil('\n');
        if(linea.indexOf("<h2>Tu IP p&uacute;blica es: ") != -1)
          Serial.println("Su IP pública es: "+
                         linea.substring(linea.indexOf("<h2>Tu IP p&uacute;blica es: ")+28, (linea.indexOf("</h2>"))));
      }
    }
    //Finaliza la conexión con el servidor
    clienteWeb.stop();                                  
  }
  else Serial.println("Problemas de conexión con el servidor");
  
  delay(10000);
}
