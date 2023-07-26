/*------------------------------------------------------------------------
  Plant Automation 360
  Firmware Ver. 1.2.0
  Date - 7 July. 2017
  Author - Nitin N. Sangale
  ------------------------------------------------------------------------*/

#include <Adafruit_ESP8266.h>
#include <ArduinoJson.h>
#include <Bounce2.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

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
#define INPUT_7  31
#define INPUT_8  33
#define INPUT_9  35
#define INPUT_10  37
#define INPUT_11  39
#define INPUT_12  41
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
Bounce Sensor_7 = Bounce();
Bounce Sensor_8 = Bounce();
Bounce Sensor_9 = Bounce();
Bounce Sensor_10 = Bounce();
Bounce Sensor_11 = Bounce();
Bounce Sensor_12 = Bounce();


//User configurable constants
#define MAC_ID "B1"
#define ESP_SSID "Shreya" // Your network name here
#define ESP_PASS "nitin.1757051" // Your network password here

#define HOST     "192.168.0.102" // Host (Server) name or IP address to contact
#define PAGE     "/time" // Web page to request (For GET request)
#define PORT     3000   // 80 = HTTP default port
#define EEPROM_SIZE EEPROM.length()

char POST_HOST[] = "192.168.0.102";
char DATA_URI[] = "/data";  //URI for DATA POST request
char REG_URI[] = "/register";  //URI for Register POST request
char PING_URI[] = "/ping";  //URI for Register POST request

char buffer[50];
String IP_Address;
unsigned long ts_secs;
int ts_msecs;
unsigned long current_time = 0;
unsigned long difference;
unsigned long secs;
unsigned long currentmillis = 0;
unsigned long last_ping_time = 0;
unsigned long ping_interval = 120000; //Set ping interval time in miliseconds

bool input_1_state, input_1_old_state;
bool input_2_state, input_2_old_state;
bool input_3_state, input_3_old_state;
bool input_4_state, input_4_old_state;
bool input_5_state, input_5_old_state;
bool input_6_state, input_6_old_state;
bool input_7_state, input_7_old_state;
bool input_8_state, input_8_old_state;
bool input_9_state, input_9_old_state;
bool input_10_state, input_10_old_state;
bool input_11_state, input_11_old_state;
bool input_12_state, input_12_old_state;

bool event_1_sent = false;
bool event_2_sent = false;
bool event_3_sent = false;
bool event_4_sent = false;
bool event_5_sent = false;
bool event_6_sent = false;
bool event_7_sent = false;
bool event_8_sent = false;
bool event_9_sent = false;
bool event_10_sent = false;
bool event_11_sent = false;
bool event_12_sent = false;

int pending_packets = 0;
int Write_Pointer = 0;
int Read_Pointer = 0;
int pending_check_interval = 0;

void setup() 
  {
    //Arduino PIN I/O Config.
    pinMode(INPUT_1,INPUT_PULLUP);
    pinMode(INPUT_2,INPUT_PULLUP);
    pinMode(INPUT_3,INPUT_PULLUP);
    pinMode(INPUT_4,INPUT_PULLUP);
    pinMode(INPUT_5,INPUT_PULLUP);
    pinMode(INPUT_6,INPUT_PULLUP);
    pinMode(INPUT_7,INPUT_PULLUP);
    pinMode(INPUT_8,INPUT_PULLUP);
    pinMode(INPUT_9,INPUT_PULLUP);
    pinMode(INPUT_10,INPUT_PULLUP);
    pinMode(INPUT_11,INPUT_PULLUP);
    pinMode(INPUT_12,INPUT_PULLUP);
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
    Sensor_7.attach(INPUT_7);
    Sensor_7.interval(1);
    Sensor_8.attach(INPUT_8);
    Sensor_8.interval(1);
    Sensor_9.attach(INPUT_9);
    Sensor_9.interval(1);
    Sensor_10.attach(INPUT_10);
    Sensor_10.interval(1);
    Sensor_11.attach(INPUT_11);
    Sensor_11.interval(1);
    Sensor_12.attach(INPUT_12);
    Sensor_12.interval(1);
    
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

    //formatROM();  //Clear EEPROM
    
    while(connectToWiFi()!=1);
    while(regWithServer()!=1);
    while(syncServerTime()!=1);

    Sensor_1.update();
    Sensor_2.update();
    Sensor_3.update();
    Sensor_4.update();
    Sensor_5.update();
    Sensor_6.update();
    Sensor_7.update();
    Sensor_8.update();
    Sensor_9.update();
    Sensor_10.update();
    Sensor_11.update();
    Sensor_12.update();
    
    input_1_old_state = Sensor_1.read();
    input_2_old_state = Sensor_2.read();
    input_3_old_state = Sensor_3.read();
    input_4_old_state = Sensor_4.read();
    input_5_old_state = Sensor_5.read();
    input_6_old_state = Sensor_6.read();
    input_7_old_state = Sensor_7.read();
    input_8_old_state = Sensor_8.read();
    input_9_old_state = Sensor_9.read();
    input_10_old_state = Sensor_10.read();
    input_11_old_state = Sensor_11.read();
    input_12_old_state = Sensor_12.read();
  }

void loop()
  {
    Sensor_1.update();
    Sensor_2.update();
    Sensor_3.update();
    Sensor_4.update();
    Sensor_5.update();
    Sensor_6.update();
    Sensor_7.update();
    Sensor_8.update();
    Sensor_9.update();
    Sensor_10.update();
    Sensor_11.update();
    Sensor_12.update();
    
    input_1_state = Sensor_1.read();
    input_2_state = Sensor_2.read();
    input_3_state = Sensor_3.read();
    input_4_state = Sensor_4.read();
    input_5_state = Sensor_5.read();
    input_6_state = Sensor_6.read();
    input_7_state = Sensor_7.read();
    input_8_state = Sensor_8.read();
    input_9_state = Sensor_9.read();
    input_10_state = Sensor_10.read();
    input_11_state = Sensor_11.read();
    input_12_state = Sensor_12.read();

    //This code will send event only once when input goes high
    if(input_1_state == false && event_1_sent == false)
      {
        event_1_sent = true;
        sendEventData(1);
      }
    if(input_1_state == true && event_1_sent == true)
      {
        event_1_sent = false;
      }

    if(input_2_state == false && event_2_sent == false)
      {
        event_2_sent = true;
        sendEventData(2);
      }
    if(input_2_state == true && event_2_sent == true)
      {
        event_2_sent = false;
      }

    if(input_3_state == false && event_3_sent == false)
      {
        event_3_sent = true;
        sendEventData(3);
      }
    if(input_3_state == true && event_3_sent == true)
      {
        event_3_sent = false;
      }

    if(input_4_state == false && event_4_sent == false)
      {
        event_4_sent = true;
        sendEventData(4);
      }
    if(input_4_state == true && event_4_sent == true)
      {
        event_4_sent = false;
      }

    if(input_5_state == false && event_5_sent == false)
      {
        event_5_sent = true;
        sendEventData(5);
      }
    if(input_5_state == true && event_5_sent == true)
      {
        event_5_sent = false;
      }

    if(input_6_state == false && event_6_sent == false)
      {
        event_6_sent = true;
        sendEventData(6);
      }
    if(input_6_state == true && event_6_sent == true)
      {
        event_6_sent = false;
      }
       if(input_7_state == false && event_7_sent == false)
      {
        event_7_sent = true;
        sendEventData(7);
      }
    if(input_7_state == true && event_7_sent == true)
      {
        event_7_sent = false;
      }

    if(input_8_state == false && event_8_sent == false)
      {
        event_8_sent = true;
        sendEventData(8);
      }
    if(input_8_state == true && event_8_sent == true)
      {
        event_8_sent = false;
      }

    if(input_9_state == false && event_9_sent == false)
      {
        event_9_sent = true;
        sendEventData(9);
      }
    if(input_9_state == true && event_9_sent == true)
      {
        event_9_sent = false;
      }

    if(input_10_state == false && event_10_sent == false)
      {
        event_10_sent = true;
        sendEventData(10);
      }
    if(input_10_state == true && event_10_sent == true)
      {
        event_10_sent = false;
      }

    if(input_11_state == false && event_11_sent == false)
      {
        event_11_sent = true;
        sendEventData(11);
      }
    if(input_11_state == true && event_11_sent == true)
      {
        event_11_sent = false;
      }

    if(input_12_state == false && event_12_sent == false)
      {
        event_12_sent = true;
        sendEventData(12);
      }
    if(input_12_state == true && event_12_sent == true)
      {
        event_12_sent = false;
      }
    
    if(pending_packets != 0 && pending_check_interval > 1000)
      {
        int res = sendPendingPackets();
        if(res == 1)
          {
            pending_packets = pending_packets - 1;
            if(pending_packets == 0)
              {
                Write_Pointer = 0;
                Read_Pointer = 0;
                formatROM();
              }
          }
        pending_check_interval = 0;  
      }
    else
      pending_check_interval++;
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

int pingServer()
  {
    digitalWrite(Comm_LED, HIGH);
    int result = 0;
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
    StaticJsonBuffer<40> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["mid"] = MAC_ID;
    root["s"] = ts_secs;
    root["m"] = ts_msecs;
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
        Serial.print(F("Sending Ping Request to Server..."));
        wifi.httpPost(POST_HOST,PING_URI,data);
        if(wifi.find())
          {
            Serial.println(F("PING SUCCESSFUL...!"));
            result = 1;
            digitalWrite(Comm_LED, LOW);
          }
        else
          {
            Serial.println(F("PING FAIL...!"));
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
          case 7:
          event_name = "Event_7";
          event_desc_type = "Seventh Event";
          event_desc = "Input 7 is changed";
          break;
        case 8:
          event_name = "Event_8";
          event_desc_type = "Eighth Event";
          event_desc = "Input 8 is changed";
          break;
        case 9:
          event_name = "Event_9";
          event_desc_type = "nineth Event";
          event_desc = "Input 9 is changed";
          break;
        case 10:
          event_name = "Event_10";
          event_desc_type = "tenth Event";
          event_desc = "Input 10 is changed";
          break;
         case 11:
          event_name = "Event_11";
          event_desc_type = "Eleven Event";
          event_desc = "Input 11 is changed";
          break; 
         case 12:
          event_name = "Event_12";
          event_desc_type = "Twelve Event";
          event_desc = "Input 12 is changed";
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
    if(result != 1)
      {
        Serial.println(F("STORING PACKET ON EEPROM...!"));
        if(storePacketOnRom(temp) == 1)
          {
             pending_packets = pending_packets + 1;
             Serial.print(pending_packets);
             Serial.println(F(" PACKET STORED ON EEPROM...!"));
          }
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

int storePacketOnRom(String packet)
{
    char ch;
    if(Write_Pointer > EEPROM_SIZE - 250)
      {
        Serial.println(F("EEPROM FULL...!"));
        return 0;
      }
    else
      {
        int c = packet.length();
        for (int x = 0; x < c; x++)
          {
            ch = packet.charAt(x);
            EEPROM.write(Write_Pointer,ch);
            Write_Pointer++;
          }
        ch = '\0';
        EEPROM.write(Write_Pointer,ch);
        Write_Pointer++;
        return 1;
      }
}

int sendPendingPackets()
{
   int i = 0;
   char buff[250];
   char ch;
   int temp_pointer;
   temp_pointer = Read_Pointer;
   int result = 0;
   
   Serial.println();
   Serial.print("Reading packet from EEPROM = ");
   ch = EEPROM.read(temp_pointer);
   while(ch != '\0')
    {
      buff[i] = ch;
      i++;
      temp_pointer++;
      ch = EEPROM.read(temp_pointer);
    }
   buff[i] = '\0';
   temp_pointer++;
   
   //Set TCP connection with Server
   digitalWrite(Comm_LED, HIGH);
   Serial.println(F("SENDING STORED PACKET"));
   Serial.println(F("Making TCP Connection With Server...!"));
   if (wifi.connectTCP(F(HOST), PORT)) 
     {
       //TCP connection set successfully
       Serial.println(F("TCP Connection Successful"));
       wifi.httpPost(POST_HOST,DATA_URI,buff);
       if(wifi.find())
         {
           Serial.println(F("STORED PACKET SENT SUCCESSFULY...!"));
           Serial.print(F("PACKET PENDING ON EEPROM = "));
           Serial.println(pending_packets-1);
           Read_Pointer = temp_pointer;
           result = 1;
           digitalWrite(Comm_LED, LOW);
         }
       else
         {
           Serial.println(F("STORED PACKET SENT FAIL...!"));
           Serial.print(F("PACKET PENDING ON EEPROM = "));
           Serial.println(pending_packets);
           result = 2;
         }
       //Close TCP connection with server  
       wifi.closeTCP();
     } 
   else 
     { 
       // TCP connect failed
       Serial.println(F("TCP connect failed...!"));
       Serial.print(F("PACKET PENDING ON EEPROM = "));
       Serial.println(pending_packets);
       result = 0;
     }
   return result;  
}

void formatROM()
{
  for (int i = 0 ; i < EEPROM_SIZE ; i++) 
    {
      EEPROM.write(i, 0);
    }
}

