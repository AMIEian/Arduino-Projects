#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include <BluetoothSerial.h>

uint8_t address[6] = {0x00,0x1c, 0x88,0x14,0x7e, 0xe6};
String MACadd = "30:AE:A4:21:BA:BA";

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_MASTER;
esp_err_t connHan;


BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  Serial.println("The device is ready!");
  SerialBT.begin("Glove ESP");
  Serial.println("Looking for device...");
  if(esp_spp_start_discovery(address) == ESP_OK){
    Serial.println("Device found!");
  }
    else{
    Serial.println("Device not found :(");
  }
  
  connHan = esp_spp_connect(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_MASTER, 3,address);
  if(connHan == ESP_OK){
    Serial.println("Device Connected!");
  }
    else{
    Serial.println("Device not connected :(");
  }
  delay(1000);
  
  if (SerialBT.hasClient() == true) {
        Serial.print("Client connected");
        //SerialBT.println("some text");
    } else {
        Serial.println("Client disconnected");
        esp_spp_disconnect(connHan);
        SerialBT.begin("Glove ESP");
        delay(1500);
    }
  
}
 
void loop() {
}
