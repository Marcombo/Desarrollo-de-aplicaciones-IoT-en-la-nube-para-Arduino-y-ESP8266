#include <ESP8266WiFi.h>
 
const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // contraseña de dicha red

IPAddress ip(192, 168, 1, 99);      //dirección IP elegida dentro de la red
IPAddress gateway(192, 168, 1, 1);  //dirección IP del gateway
IPAddress subnet(255, 255, 255, 0); //mascara de red
 
WiFiServer servidorWeb(80);
WiFiClient clienteWeb;
 
void setup() {
  Serial.begin(115200);

  //Se inicia la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");   
  WiFi.config(ip, gateway, subnet);  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Se arranca el servidor Web
  servidorWeb.begin();
  Serial.println("Servidor web arrancado");
}
 
void loop() {
  clienteWeb = servidorWeb.available();
  //Se espera que llegue un cliente
  if (clienteWeb) {
    boolean lineaEnBlanco = true;     // La cabecera de una petición HTTP (línea de solicitud + encabezados) finaliza con una linea en blanco

    //El cliente está conectado
    while (clienteWeb.connected()) {
      //el cliente ha transmitido datos
      if (clienteWeb.available()) {
        char c = clienteWeb.read();
        if (c == '\n' && lineaEnBlanco){//la petición es de tipo GET sin cuerpo
          clienteWeb.println("HTTP/1.1 200 OK");
          clienteWeb.println("Content-Type: text/html");
          clienteWeb.println("Connection: close");
          clienteWeb.println();
          clienteWeb.println("<!DOCTYPE HTML>");
          clienteWeb.println("<head>");
          clienteWeb.println("<meta charset='UTF-8'>");
          clienteWeb.println("<title>Servidor Web ESP8266</title>");
          clienteWeb.println("</head>");
          clienteWeb.println("<html>");
          clienteWeb.println("<body>");
          clienteWeb.println("<h1>Bienvenido al servidor Web de mi ESP8266</h1>");
          clienteWeb.println("</body>");
          clienteWeb.println("</html>");
          break;
        }
        if (c == '\n') lineaEnBlanco = true; 
        else if (c != '\r') lineaEnBlanco = false; 
      }
    }
    //Cerramos la conexión para asegurar  que los datos se envían
    delay(10);         
    clienteWeb.stop();
  }
}
