#include "FS.h"
#include "SPIFFS.h"

#define SERIAL_SIZE_RX  1024
#define FORMAT_SPIFFS_IF_FAILED true

bool fileReceiving = false;
bool fileReceived = false;
bool fileCreated = false;
int bufferIndex = 0;
long lastByteTime = 0;
int fileEndTime = 5000;

//char buff[512];
static uint8_t buff[512];

void setup() 
  {
    Serial.begin(115200);
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
      {
        Serial.println("SPIFFS Mount Failed");
        return;
      }
  }

void loop() 
  {    
    if(fileReceiving == true)
      {
        if(millis() - lastByteTime >= fileEndTime)
          {
            if(bufferIndex != 0)
              {
                if(fileCreated == false)
                  //writeFile(SPIFFS, "/announcement.mp3", buff);
                  writeFile(SPIFFS, "/announcement.mp3");
                else
                  //appendFile(SPIFFS, "/announcement.mp3", buff);
                  appendFile(SPIFFS, "/announcement.mp3");
                clearBuff();
                bufferIndex = 0;
              }
            fileReceived = true;
            fileReceiving = false;
          }
      }
    if(fileReceived == true)
      {
        readFile(SPIFFS, "/announcement.mp3");
        Serial.println();
        deleteFile(SPIFFS, "/announcement.mp3");
        fileReceived = false;
        fileCreated = false;
      }
    serialEvent();
  }

/*
void loop() 
  {    
    if(fileReceiving == true)
      {
        if(millis() - lastByteTime >= fileEndTime)
          {
            if(bufferIndex != 0)
              {
                if(fileCreated == false)
                  writeFile(SPIFFS, "/announcement.mp3", buff);
                else
                  appendFile(SPIFFS, "/announcement.mp3", buff);
                clearBuff();
                bufferIndex = 0;
              }
            fileReceived = true;
            fileReceiving = false;
          }
      }
    if(fileReceived == true)
      {
        readFile(SPIFFS, "/announcement.mp3");
        Serial.println();
        deleteFile(SPIFFS, "/announcement.mp3");
        fileReceived = false;
        fileCreated = false;
      }
    serialEvent();
  }
*/

void serialEvent() 
  {
    while(Serial.available()) 
      {
        lastByteTime = millis();
        fileReceiving = true;
        buff[bufferIndex] = Serial.read();       
        bufferIndex = bufferIndex + 1;
        if(bufferIndex == 512)
          {            
            if(fileCreated == false)
              {
                //writeFile(SPIFFS, "/announcement.mp3", buff);
                writeFile(SPIFFS, "/announcement.mp3");
                fileCreated = true;
              }
            else
              {
                //appendFile(SPIFFS, "/announcement.mp3", buff);
                appendFile(SPIFFS, "/announcement.mp3");
              }
            clearBuff();
            bufferIndex = 0;
          }
      }
  }

/*
void serialEvent() 
  {
    while(Serial.available()) 
      {
        lastByteTime = millis();
        fileReceiving = true;
        buff[bufferIndex] = Serial.read();       
        bufferIndex = bufferIndex + 1;
        if(bufferIndex == 512)
          {            
            if(fileCreated == false)
              {
                writeFile(SPIFFS, "/announcement.mp3", buff);
                fileCreated = true;
              }
            else
              {
                appendFile(SPIFFS, "/announcement.mp3", buff);
              }
            clearBuff();
            bufferIndex = 0;
          }
      }
  }
*/

void clearBuff()
{
  for(int i = 0; i < 512; i++)
    {
      buff[i] = 0;
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

//void writeFile(fs::FS &fs, const char * path, const char * message){
void writeFile(fs::FS &fs, const char * path){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.write(buff, 512)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    /*
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    */
    file.close();
}

//void appendFile(fs::FS &fs, const char * path, const char * message){
void appendFile(fs::FS &fs, const char * path){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }

    if(file.write(buff, 512)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    /*
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    */
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
   Serial.printf("Deleting file: %s\r\n", path);
   if(fs.remove(path)){
      Serial.println("− file deleted");
   } else {
      Serial.println("− delete failed");
   }
}
