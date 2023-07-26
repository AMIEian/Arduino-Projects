/*------------------------------------------------------------------------
  Plant Automation 360
  Firmware Ver. 1.0
  Date - 21 Jan. 2017
  Author - Nitin N. Sangale
  ------------------------------------------------------------------------*/

#include <Adafruit_ESP8266.h>
#include <ArduinoJson.h>
#include <Bounce2.h>
#include <SoftwareSerial.h>

//Arduino Pin Definitions
#define ESP_RX   10
#define ESP_TX   11
#define ESP_RST  4
#define INPUT_1  5
#define INPUT_2  6
#define INPUT_3  7
#define INPUT_4  8
#define INPUT_5  9
#define INPUT_6  12
#define LED  13
#define Comm_LED  A0
#define WiFi_LED  A1

//Software Serial Object
SoftwareSerial softser(ESP_RX, ESP_TX);
//ESP8266 WiFi Object
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);
//Debouncer Objects
Bounce Sensor_1 = Bounce();
Bounce Sensor_2 = Bounce();
Bounce Sensor_3 = Bounce();
Bounce Sensor_4 = Bounce();
Bounce Sensor_5 = Bounce();
Bounce Sensor_6 = Bounce();

//User configurable constants
#define MAC_ID "B1"
#define ESP_SSID "AMIEian" // Your network name here
#define ESP_PASS "nitin.1757051" // Your network password here

#define HOST     "192.168.0.104" // Host (Server) name or IP address to contact
#define PAGE     "/time" // Web page to request (For GET request)
#define PORT     3000   // 80 = HTTP default port

char POST_HOST[] = "192.168.0.104";
char DATA_URI[] = "/data";  //URI for DATA POST request
char REG_URI[] = "/register";  //URI for Register POST request

char buffer[50];
String IP_Address;
unsigned long ts_secs;
int ts_msecs;
unsigned long current_time = 0;
int difference;
int secs;

bool input_1_state, input_1_old_state;
bool input_2_state, input_2_old_state;
bool input_3_state, input_3_old_state;
bool input_4_state, input_4_old_state;
bool input_5_state, input_5_old_state;
bool input_6_state, input_6_old_state;

void setup() 
  {
    //Arduino PIN I/O Config.
    pinMode(INPUT_1,INPUT_PULLUP);
    pinMode(INPUT_2,INPUT_PULLUP);
    pinMode(INPUT_3,INPUT_PULLUP);
    pinMode(INPUT_4,INPUT_PULLUP);
    pinMode(INPUT_5,INPUT_PULLUP);
    pinMode(INPUT_6,INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    pinMode(Comm_LED, OUTPUT);
    pinMode(WiFi_LED, OUTPUT);
    
    digitalWrite(Comm_LED, LOW);
    digitalWrite(WiFi_LED, LOW);
    
    //Attach Debouncer to input pins
    Sensor_1.attach(INPUT_1);
    Sensor_1.interval(1);
    Sensor_2.attach(INPUT_2);
    Sensor_2.interval(1);
    Sensor_3.attach(INPUT_3);
    Sensor_3.interval(1);
    Sensor_4.attach(INPUT_4);
    Sensor_4.interval(1);
    Sensor_5.attach(INPUT_5);
    Sensor_5.interval(1);
    Sensor_6.attach(INPUT_6);
    Sensor_6.interval(1);
    
    // Flash LED on power-up
    for (uint8_t i = 0; i < 2; i++) 
      {
        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
        delay(1000);
      }
  
    softser.begin(9600); // Software serial connection to ESP8266
    Serial.begin(57600); //while (!Serial); // UART serial for debug
    
    while(connectToWiFi()!=1);
    while(regWithServer()!=1);
    while(syncServerTime()!=1);
    
    Sensor_1.update();
    Sensor_2.update();
    Sensor_3.update();
    Sensor_4.update();
    Sensor_5.update();
    Sensor_6.update();
    
    input_1_old_state = Sensor_1.read();
    input_2_old_state = Sensor_2.read();
    input_3_old_state = Sensor_3.read();
    input_4_old_state = Sensor_4.read();
    input_5_old_state = Sensor_5.read();
    input_6_old_state = Sensor_6.read();
  }

void loop()
  {
    /*
    if(current_time != millis())
      {
        difference = 0;
        difference = millis() - current_time;
        current_time = millis();
        secs = 0;
        secs = difference / 1000;
        ts_msecs = difference % 1000;
        ts_secs = ts_secs + secs;
      }
    */
    Sensor_1.update();
    Sensor_2.update();
    Sensor_3.update();
    Sensor_4.update();
    Sensor_5.update();
    Sensor_6.update();
    
    input_1_state = Sensor_1.read();
    input_2_state = Sensor_2.read();
    input_3_state = Sensor_3.read();
    input_4_state = Sensor_4.read();
    input_5_state = Sensor_5.read();
    input_6_state = Sensor_6.read();
    
    if(input_1_state != input_1_old_state)
      {
        input_1_old_state = input_1_state;
        sendEventData(1);
      }
    if(input_2_state != input_2_old_state)
      {
        input_2_old_state = input_2_state;
        sendEventData(2);
      }
    if(input_3_state != input_3_old_state)
      {
        input_3_old_state = input_3_state;
        sendEventData(3);
      }
    if(input_4_state != input_4_old_state)
      {
        input_4_old_state = input_4_state;
        sendEventData(4);
      }
    if(input_5_state != input_5_old_state)
      {
        input_5_old_state = input_5_state;
        sendEventData(5);
      }
    if(input_6_state != input_6_old_state)
      {
        input_6_old_state = input_6_state;
        sendEventData(6);
      }  
  }

int connectToWiFi()
  {
    int result = 0;
    Serial.println(F("Connecting to WiFi..."));
    if (wifi.connectToAP(F(ESP_SSID), F(ESP_PASS))) 
      {
        //Connected to WiFi get IP address
        Serial.println(F("Connected to WiFi Network...!"));
        Serial.println(F("Obtaining IP addr..."));
        softser.println(F("AT+CIFSR"));
        String temp = "NO_IP";
        temp = softSerResponse();
        if(temp != "NO_IP")
          {
            int startpos = temp.indexOf('"')+1;
            int endpos = temp.indexOf('"',startpos);
            IP_Address = temp.substring(startpos,endpos);
          }
        Serial.print(F("IP Address we got is = "));
        Serial.println(IP_Address);
        result = 1; 
        digitalWrite(WiFi_LED, HIGH);
      }
    else 
      { 
            // WiFi connection failed
            Serial.println(F("WiFi Connect Fail...!"));
            result = 0;
      }
    return result;  
  }

int regWithServer()
  {
    digitalWrite(Comm_LED, HIGH);
    int result = 0;
    StaticJsonBuffer<40> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["mid"] = MAC_ID;
    root["ip"] = IP_Address;
    String request_data;
    root.printTo(request_data);
    char data[40];
    request_data.toCharArray(data,40);
    //Set TCP connection with Server
    Serial.println(F("Making TCP Connection With Server...!"));
    if (wifi.connectTCP(F(HOST), PORT)) 
      {
        //TCP connection set successfully
        Serial.println(F("TCP Connection Successful"));
        Serial.print(F("Registering Machine to Server..."));
        wifi.httpPost(POST_HOST,REG_URI,data);
        if(wifi.find())
          {
            Serial.println(F("REGISTRATION SUCCESSFUL...!"));
            result = 1;
            digitalWrite(Comm_LED, LOW);
          }
        else
          {
            Serial.println(F("REGISTRATION FAIL...!"));
            result = 2;
          }
        //Close TCP connection with server  
        wifi.closeTCP();
      } 
    else 
      { 
        // TCP connect failed
        Serial.println(F("TCP connect failed...!"));
        result = 0;
      }
    return result;  
  }

int sendEventData(int event)
  {
    digitalWrite(Comm_LED, HIGH);
    int result = 0;
    String event_name = "";
    String event_desc_type = "";
    String event_desc = "";
    
    switch(event)
      {
        case 1:
          event_name = "Event_1";
          event_desc_type = "First Event";
          event_desc = "Input 1 is changed";
          break;
        case 2:
          event_name = "Event_2";
          event_desc_type = "Second Event";
          event_desc = "Input 2 is changed";
          break;
        case 3:
          event_name = "Event_3";
          event_desc_type = "Thired Event";
          event_desc = "Input 3 is changed";
          break;
        case 4:
          event_name = "Event_4";
          event_desc_type = "Fourth Event";
          event_desc = "Input 4 is changed";
          break;
         case 5:
          event_name = "Event_5";
          event_desc_type = "Fifth Event";
          event_desc = "Input 5 is changed";
          break; 
         case 6:
          event_name = "Event_6";
          event_desc_type = "Sixth Event";
          event_desc = "Input 6 is changed";
          break; 
         default:
          event_name = "Unknown Event";
          event_desc_type = "Unknown Event";
          event_desc = "Unknown Reason of event trigger";
          break;
      }
    
    if(current_time != millis())
      {
        difference = 0;
        difference = millis() - current_time;
        current_time = millis();
        secs = 0;
        secs = difference / 1000;
        ts_msecs = difference % 1000;
        ts_secs = ts_secs + secs; 
        Serial.print("SECS = ");
        Serial.println(ts_secs);   
        Serial.print("MSECS = ");
        Serial.println(ts_msecs);
      }
    
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["mid"] = MAC_ID;
    JsonArray& data = root.createNestedArray("data");
    
    StaticJsonBuffer<200> jsonBuffer1;
    JsonObject& root1 = jsonBuffer1.createObject();
    root1["event"]  = event_name;
    root1["s"] = ts_secs;
    root1["m"] = ts_msecs;
    root1["desc_type"] = event_desc_type;
    root1["desc"] = event_desc;
    
    data.add(root1);

    String temp;
    root.printTo(temp);
    char event_data[250];
    temp.toCharArray(event_data,250);
    
    //Set TCP connection with Server
    Serial.println(F("Making TCP Connection With Server...!"));
    if (wifi.connectTCP(F(HOST), PORT)) 
      {
        //TCP connection set successfully
        Serial.println(F("TCP Connection Successful"));
        Serial.print(F("Sending event data to Server..."));
        wifi.httpPost(POST_HOST,DATA_URI,event_data);
        if(wifi.find())
          {
            Serial.println(F("EVENT DATA SENT SUCCESSFULY...!"));
            result = 1;
            digitalWrite(Comm_LED, LOW);
          }
        else
          {
            Serial.println(F("EVENT DATA SENT FAIL...!"));
            result = 2;
          }
        //Close TCP connection with server  
        wifi.closeTCP();
      } 
    else 
      { 
        // TCP connect failed
        Serial.println(F("TCP connect failed...!"));
        result = 0;
      }
    return result;  
  }

int syncServerTime()
  {
    digitalWrite(Comm_LED, HIGH);
    int result = 0;
    //Set TCP connection with Server
    Serial.println(F("Making TCP Connection With Server...!"));
    if (wifi.connectTCP(F(HOST), PORT)) 
      {
        //TCP connection set successfully
        Serial.println(F("TCP Connection Successful"));
        Serial.println(F("Requesting for time..."));
        ts_secs = 0;
        ts_msecs = 0;
        requestTime();
        String temp = softSerResponse();
        ts_secs = temp.substring(temp.lastIndexOf("s")+3,temp.lastIndexOf(",")).toInt();
        ts_msecs = temp.substring(temp.lastIndexOf(".")+1,temp.lastIndexOf(0x7D)).toInt();
        
        if(ts_secs != 0)
          {
            Serial.print(F("Time we got is = "));
            Serial.print(ts_secs);
            Serial.print(F(" Seconds "));
            Serial.print(ts_msecs);
            Serial.println(F(" Milli Seconds"));
            result = 1;
            digitalWrite(Comm_LED, LOW);
            current_time = millis();
          }
        else
          {
            Serial.println(F("Wrong String"));
            result = 2;
          }

        //Close TCP connection with server  
        wifi.closeTCP();
      } 
    else 
      { 
        // TCP connect failed
        Serial.println(F("TCP connect failed...!"));
        result = 0;
      }
    return result;  
  }

void requestTime()
  {
    softser.print(F("AT+CIPSEND="));
    softser.println(25 + strlen_P((Pchr *)F(PAGE)) + strlen_P((Pchr *)F(HOST)));
    if(wifi.find(F("> "))) 
      { 
        // Wait for prompt
        softser.print(F("GET ")); // 4
        softser.print(F(PAGE));
        softser.print(F(" HTTP/1.1\r\nHost: ")); // 17
        softser.print(F(HOST));
        softser.print(F("\r\n\r\n")); // 4
      }
  }
  
String softSerResponse()
  {
    String response = "";
    
    long int time = millis();
    
    while( (time+1000) > millis())
    {
      while(softser.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = softser.read(); // read the next character.
        response+=c;
      }  
    }
    Serial.print(response);
    
    return response;
  }
