// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

#define led 2

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = "imis"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// Server details
const char server[] = "sendtofirebase.000webhostapp.com"; // domain name: example.com, maker.ifttt.com, etc
//char resource[200] = "/upload.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=28.854069709&lon=";
char resource[200] = "/download.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=";
char longitude_str[6]="&lon=";

char variable[20];
double longitude = 13.854069709;
double latitude = 14.854069709;

const int  port = 80; // server port number

TinyGsm modem(SerialAT);
// TinyGSM Client for Internet connection
TinyGsmClient client(modem);
// HHTP Client for server connection
HttpClient http(client, server, port);

void setup() 
  {
    pinMode(led, INPUT);
    digitalWrite(led, LOW);
    // Set serial monitor debugging window baud rate to 115200
    SerialMon.begin(115200);
    modemInit();
  }

void loop() 
  {  
    SerialMon.print("Connecting to APN: ");
    SerialMon.print(apn);
    if(!modem.gprsConnect(apn, gprsUser, gprsPass))
      SerialMon.println(" fail");
    else 
      {
        SerialMon.println(" OK");
        SerialMon.print("Connecting to ");
        SerialMon.print(server);
        if(!client.connect(server, port))
          SerialMon.println(" fail");
        else 
          {
            SerialMon.println(" OK");
            SerialMon.println("Performing HTTP GET request... ");
            /*
             * Our final resource string is = "/upload.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=14.854069709&lon=13.854069709";
             * Currently our resource is = "/download.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=";
             * Append 14.854069709 to it
             */
            dtostrf(latitude, 12, 9, variable);
            SerialMon.println(variable);
            strcat(resource,variable);
            /*
             * Now our resource string is = "/download.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=14.854069709"
             * Append "&lon=" to it
             */
            strcat(resource,longitude_str);
            /*
             * Now our resource string is = "/download.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=14.854069709&lon="
             * Append 13.854069709 to it
             */
            dtostrf(longitude, 12, 9, variable);
            SerialMon.println(variable);
            strcat(resource,variable);
            /*
             * Now our final resource string is ready = "/download.php?adr=https://test-iot-a3656.firebaseio.com&key=xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu&deviceid=0&lat=14.854069709&lon=13.854069709"
             * Repeat same steps for another parameters also...
             */
            SerialMon.println(resource);
            int err = http.get(resource);
            if(err != 0) 
              {
                SerialMon.println("failed to connect");
                delay(10000);
                return;
              }
          
            int status = http.responseStatusCode();
            SerialMon.println(status);
            if(!status) 
              {
                delay(10000);
                return;
              }
          
            while (http.headerAvailable()) 
              {
                String headerName = http.readHeaderName();
                String headerValue = http.readHeaderValue();
                //SerialMon.println(headerName + " : " + headerValue);
              }
          
            int length = http.contentLength();
            if (length >= 0) 
              {
                SerialMon.print("Content length is: ");
                SerialMon.println(length);
              }
              
            if (http.isResponseChunked()) 
              SerialMon.println("The response is chunked");
          
            String body = http.responseBody();
            SerialMon.println("Response:");
            SerialMon.println(body);
            if(body == "1")
              digitalWrite(led, HIGH);
            if(body == "0")
              digitalWrite(led, LOW);
          
            SerialMon.print("Body length is: ");
            SerialMon.println(body.length());
          
            // Shutdown
          
            http.stop();
            SerialMon.println("Server disconnected");
          
            modem.gprsDisconnect();
            SerialMon.println("GPRS disconnected");
          
            // Do nothing forevermore
            while (true)
              delay(1000);
        }
    }
  }
