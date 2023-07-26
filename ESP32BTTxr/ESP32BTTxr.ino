#include "BluetoothSerial.h"
BluetoothSerial UnleashedBT;
void setup ()
{
  Serial.begin(115200);
  uint8_t new_mac [8] = {0x00,0x1c, 0x88,0x14,0x7e, 0xe6};
  esp_base_mac_addr_set(new_mac);
  UnleashedBT.begin("ESP32_Unleashed");
}

void loop ()
{
  UnleashedBT.println("XXXXXXX");
  delay(2000);
}
