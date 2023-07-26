#define TINY_GSM_MODEM_SIM7600

#include <TinyGsmClient.h>
#include <WiFiClient.h>   
#include <WiFi.h>         
#include <WiFiServer.h>

#define SerialMon Serial
HardwareSerial SerialAT(1);
#define TINY_GSM_RX_BUFFER 2048
  
const char apn[]  = "jionet"; // Change this to your Provider details
const char gprsUser[] = "";
const char gprsPass[] = "";
char server[] = "vsh.pp.ua"; // Change this to your selection
String resource = "/TinyGSM/logo.txt";
const int  port = 80;
unsigned long timeout;

// Variables and desired credentials.
const char *ssid     = "ESP-32";             // Access Point - SSID (Name)
const char *password = "123456789";            // Access Point - Password
const int APPort     = 8083;                 // Access Point -  Port

WiFiServer APserver(80);
TinyGsm modem(SerialAT);
TinyGsmClient Webclient(modem);

void setup() 
  {
    SerialMon.begin(115200);
    delay(10);
    SerialMon.println("Wait...");
    SerialAT.begin(115200,SERIAL_8N1,16,17, false);
    delay(600);
    SerialMon.println("Initializing modem...");

    delay(10);
  
    WiFi.onEvent(WiFiEvent);             // set event-handler for WiFi
    WiFi.softAP(ssid, password);         // define credentials 
    IPAddress myIP = WiFi.softAPIP();    // inform console about own IP-Adress
    SerialMon.print("AccessPoint IP address: ");
    SerialMon.println(myIP);
    APserver.begin();              // AccessPoint with defined Port 
    SerialMon.println("WiFi Server started");

    SerialMon.println("Initializing modem...");
    modem.init();                           // if you don't need the complete restart
    String modemInfo = modem.getModemInfo();
    SerialMon.print("ModemInfo: ");     // inform console about Modem info
    SerialMon.println(modemInfo);

    SerialMon.print("Modemstatus: ");   // inform console about Modem info
    SerialMon.println(modem.getSimStatus());
  }

void loop() 
  {
     WiFiClient client = APserver.available();   // listen for incoming clients
     if (client) 
       {                               // if you get a client,
         process(client);                        // Process request
         delay(10);
         client.stop();                          // Close connection and free resources.
         Serial.println("Client Disconnected.");
       }
  }

void process(WiFiClient client) 
  {
    Serial.println("New Session.");            // print a message out the serial port
    String currentLine = "";                   // make a String to hold incoming data from the client
    while (client.connected())                 // loop while the client's connected
      {               
        if (client.available())               // if there's bytes to read from the client,
          {                
            char c = client.read();              // read a byte, then
            Serial.write(c);                     // print it out the serial monitor
            if (c == '\n')                       // if the byte is a newline character
              {                     
                // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:
                if (currentLine.length() == 0) 
                  {
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();
                    // the content of the HTTP response follows the header:
                    client.print("Klick <a href=\"/vsh.pp.ua\">here</a> to read sample page.<br>");
                    // The HTTP response ends with another blank line:
                    client.println();
                    // break out of the while loop:
                    break;
                  } 
                else        // if you got a newline, then clear currentLine:
                  {                
                    currentLine = "";
                  }
              }
          
          else if (c != '\r') // if you got anything else but a carriage return character,
            {     
              currentLine += c;       // add it to the end of the currentLine
            }
          
          // Check to see if the client request was "GET //vsh.pp.ua":
          if (currentLine.endsWith(" HTTP/1.1")) 
            {
              // First line of HTTP request looks like "GET /path HTTP/1.1"
              // Retrieve the "/path" part by finding the spaces or "/"
              int addr_start = currentLine.indexOf(' ');
              int addr_midl = currentLine.indexOf('/', addr_start + 2);              
              int addr_end = currentLine.indexOf('HTTP');
              String req_server = currentLine.substring(addr_start + 2, addr_midl);
              SerialMon.println("---------------------------");  
              SerialMon.println(addr_start);  
              SerialMon.println(addr_midl);  
              SerialMon.println(addr_end);  
              SerialMon.println("---------------------------");  
              SerialMon.print("req_server.length():  ");   
              SerialMon.println(req_server.length());
              SerialMon.println(req_server);
              
              if (req_server.length() == 0) 
                {
                  // No server defined -> Some Help
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println();
                  // the content of the HTTP response follows the header:
                  client.print("Klick <a href=\"/vsh.pp.ua/TinyGSM/logo.txt\">here</a> um das Logo aufzurufen.<br>");
                  // The HTTP response ends with another blank line:
                  client.println();
                  // break out of the while loop:
                  break;
                }
              String req_page = currentLine.substring(addr_midl, addr_end - 4);
              strcpy(server, req_server.c_str());
              resource = req_page;
    
              SerialMon.print("resource:  ");   
              SerialMon.println(resource);
              
              SerialMon.print("Connecting to ");
              SerialMon.print(server);
              if (!Webclient.connect(server, port)) 
                {
                    SerialMon.println(" fail");
                    delay(10000);
                    return;
                } 
              SerialMon.println(" OK");
    
              // Make a HTTP GET request:
              SerialMon.println("Performing HTTP GET request...");
              Webclient.print(String("GET ") + resource + " HTTP/1.1\r\n");
              Webclient.print(String("Host: ") + server + "\r\n");
              Webclient.print("Connection: close\r\n\r\n");
              Webclient.println();
    
              unsigned long timeout = millis();
              while (Webclient.connected() && millis() - timeout < 10000L) 
                {
                  // Print available data
                  while (Webclient.available()) 
                    {
                      char c = Webclient.read();
                      //SerialMon.print(c);
                      client.print(c);        // Ausgabe gleich auf den Client
                      timeout = millis();
                    }
                }
              SerialMon.println("Page Displayed ...!");
                // Verindung zum Web-Server abbauen
              Webclient.stop();
              SerialMon.println(F("Web-Server disconnected"));
           }
      }
    }
  }
  
//  
//  SerialMon.println("Performing HTTP POST request...");
//  String httpRequestData = "key=a@4K%3&distance="+ String(distance) +"";
//  client.print(String("POST ") + resource + " HTTP/1.1\r\n");
//  client.print(String("Host: ") + server + "\r\n");
//  client.println("Connection: close");
//  client.println("Content-Type: application/x-www-form-urlencoded");
//  client.print("Content-Length: ");
//  client.println(httpRequestData.length());
//  client.println();
//  client.println(httpRequestData);
//  timeout = millis();
//  while (client.connected() && millis() - timeout < 10000L) {
//    while (client.available()) {
//      char c = client.read();
//      SerialMon.print(c);
//      timeout = millis();
//    }
//  }
//  SerialMon.println();
//  client.stop();
//  SerialMon.println(F("Server disconnected"));
//  modem.gprsDisconnect();
//  SerialMon.println(F("GPRS disconnected"));
//}

void WiFiEvent(WiFiEvent_t event) 
{
  // Handler of WIFI-Event 
  switch (event) 
    {
      case SYSTEM_EVENT_WIFI_READY:
          Serial.println("SYSTEM_EVENT_WIFI_READY");
          break;
    
      case SYSTEM_EVENT_SCAN_DONE:
          Serial.println("SYSTEM_EVENT_SCAN_DONE");
          break;
    
      case SYSTEM_EVENT_STA_START:
          Serial.println("SYSTEM_EVENT_STA_START");
          break;
    
      case SYSTEM_EVENT_STA_STOP:
          Serial.println("SYSTEM_EVENT_STA_STOP");
          break;
    
      case SYSTEM_EVENT_STA_CONNECTED://or STARTED ?
          Serial.println("SYSTEM_EVENT_STA_CONNECTED");
          break;
    
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("SYSTEM_EVENT_STA_DISCONNECTED");
          break;
    
      case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
          Serial.println("SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
          break;
    
      case SYSTEM_EVENT_STA_GOT_IP:
          Serial.println("SYSTEM_EVENT_STA_GOT_IP");
          break;
    
      case SYSTEM_EVENT_STA_LOST_IP:
          Serial.println("SYSTEM_EVENT_STA_LOST_IP");
          break;
    
      case SYSTEM_EVENT_AP_START:
          Serial.println("SYSTEM_EVENT_AP_START");
          break;
    
      case SYSTEM_EVENT_AP_STOP:
          Serial.println("SYSTEM_EVENT_AP_STOP");
          break;
    
      case SYSTEM_EVENT_AP_STACONNECTED:
          Serial.println("SYSTEM_EVENT_AP_STACONNECTED");
          break;
    
      case SYSTEM_EVENT_AP_STADISCONNECTED:
          Serial.println("SYSTEM_EVENT_AP_STADISCONNECTED");
          modem.gprsDisconnect();
          SerialMon.println(F("GPRS disconnected"));
          // Do nothing forevermore
          SerialMon.println("www.thingsmobile.com user lora@felmayer.de");
          break;
    
      case SYSTEM_EVENT_AP_STAIPASSIGNED:
          Serial.println("SYSTEM_EVENT_AP_STAIPASSIGNED");
          // wenn eine WLAN-Verbindung aufgebaut ist !!!
          // Netzwerk initialisieren und Modem tiiii tööööö ti tit ti so wie bei alten Modem 14400 ... 
          SerialMon.print("Waiting for network...");
          if (!modem.waitForNetwork(240000L)) 
            {
                SerialMon.println(" fail");
                delay(10000);
                return;
            }
          SerialMon.println(" OK");
          if (modem.isNetworkConnected()) 
            {
                SerialMon.println("Network connected");
            }
          SerialMon.print(F("Connecting to APN: "));
          SerialMon.print(apn);
          if (!modem.gprsConnect(apn, gprsUser, gprsPass)) 
            {
                SerialMon.println(" fail");
                delay(10000);
                return;
             }
           SerialMon.println(" OK");  
           break;
    
      case SYSTEM_EVENT_AP_PROBEREQRECVED:
          SerialMon.println("SYSTEM_EVENT_AP_PROBEREQRECVED");
          break;
    
      default:
          SerialMon.println("UNKNOWN EVENT: " + event);
          break;
    }
}
