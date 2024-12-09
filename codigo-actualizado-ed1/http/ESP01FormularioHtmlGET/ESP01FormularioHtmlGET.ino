#include <ESP8266WiFi.h>
 
const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // contraseña de dicha red

IPAddress ip(192, 168, 1, 99);      //dirección IP elegida dentro de la red
IPAddress gateway(192, 168, 1, 1);  //dirección IP del gateway
IPAddress subnet(255, 255, 255, 0); //mascara de red
 
WiFiServer servidorWeb(80);
WiFiClient clienteWeb;

String resultadoEjecucion;
 
void setup() {
  Serial.begin(115200);

  pinMode(0, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

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
  resultadoEjecucion = "";
  clienteWeb = servidorWeb.available();
  //Se espera que llegue un cliente
  if (clienteWeb) {
    boolean lineaEnBlanco = true;     // El encabezado de una petición HTTP (línea de solicitud + cabeceras) finaliza con una linea en blanco
    String encabezado = "";             // petición HTTP
    String cuerpo = "";               //cuerpo de la petición
    int tamanioCuerpo = -1;           //tamaño del cuerpo de la petición

    //El cliente está conectado
    while (clienteWeb.connected()) {
      //el cliente ha transmitido datos
      if (clienteWeb.available()) {
        char c = clienteWeb.read();
        Serial.write(c);   // Se muestra en el monitor serie
        encabezado += c;
        if (c == '\n' && lineaEnBlanco && encabezado.startsWith("GET")){//la petición es de tipo POST sin cuerpo
          controlGPIOs(encabezado);
          enviarMensajeRespuesta();
          break;
        }
        else if (c == '\n' && lineaEnBlanco && encabezado.startsWith("POST")) { //la petición es de tipo GET con cuerpo
          //Se obtiene el tamaño de cuerpo
          String temp = encabezado.substring(encabezado.indexOf("Content-Length:") + 15);
          temp.trim();
          tamanioCuerpo = temp.toInt();
          //se obtiene el cuerpo de la petición
          while(tamanioCuerpo-- > 0)
          {
            c = clienteWeb.read();
            Serial.write(c);
            cuerpo += c;
          }
          Serial.write("\n\n");
          enviarMensajeRespuesta();
          break;
      }
        if (c == '\n') lineaEnBlanco = true; 
        else if (c != '\r') lineaEnBlanco = false; 
      }
    }
    //Se cierra la conexión para asegurar  que los datos se envían
    delay(10);         
    clienteWeb.stop();
  }
}

void controlGPIOs(String encabezado){
  int gpio = encabezado.substring(encabezado.indexOf("GPIO=")+5, encabezado.indexOf("GPIO=")+6).toInt();
  int valor = digitalRead(gpio);
  if (valor == 1) resultadoEjecucion = "El GPIO" + String(gpio) + " tiene un valor ALTO";
  else resultadoEjecucion = "El GPIO" + String(gpio) + " tiene un valor BAJO";
}

//Devuelve un mensaje de respuesta con una página HTML
void enviarMensajeRespuesta(){
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
  clienteWeb.println("<form method='GET'>");
  clienteWeb.println("<fieldset style='width:200px'>");
  clienteWeb.println("<legend>Control GPIOs ESP-01</legend>");
  clienteWeb.println("GPIO: ");
  clienteWeb.println("<select name='GPIO'>");
  clienteWeb.println("<option value='0'>GPIO0</option>");
  clienteWeb.println("<option value='2'>GPIO2</option>");
  clienteWeb.println("</select>");
  clienteWeb.println("<br><br>");
  clienteWeb.println("<input type='submit' value='Enviar'>");
  clienteWeb.println("</fieldset>");
  clienteWeb.println("</form>");
  clienteWeb.println("<br>");
  clienteWeb.println(resultadoEjecucion);
  clienteWeb.println("</body>");
  clienteWeb.println("</html>");
}

