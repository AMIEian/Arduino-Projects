#include <WiFi.h>
#include "FirebaseESP32.h"

#define FIREBASE_HOST "test-iot-a3656.firebaseio.com"
#define FIREBASE_AUTH "xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu"

#define WIFI_SSID "AMIEian-Pocket"
#define WIFI_PASSWORD "nitin.1757051"

FirebaseData firebaseData;
FirebaseData firebaseData2;

TaskHandle_t Task1;

String Path = "/Test123";

int count = 0;

void Task1code( void * parameter) {


  for (;;) {

    if (!Firebase.readStream(firebaseData))
    {
      Serial.print("Read stream error: ");
      Serial.println(firebaseData.errorReason());
      Serial.println();
    }

    if (firebaseData.streamTimeout())
    {
      Serial.println("Stream timeout");
      Serial.println();
    }

    if (firebaseData.streamAvailable())
    {
      Serial.print("Stream data: ");
      String data = firebaseData.stringData();
      Serial.println(data);
      Serial.println();
    }

    vTaskDelay(10);
  }
}

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, Path))
  {

    Serial.println("Can't begin stream connection...");
    Serial.println(firebaseData.errorReason());

  }

  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 0, &Task1, 0/* core 0 */);
}


void loop()
{
  count++;
  String data = "Hello " + String(count);
  Firebase.set(firebaseData2, Path,  data);
  delay(2000);
}
