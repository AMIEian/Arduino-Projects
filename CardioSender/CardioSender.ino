#include <WiFi.h>
#include <FirebaseESP32.h>
#include <FirebaseJson.h>

#define FIREBASE_HOST "test-iot-a3656.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "xdRGJFrtNTR2ixpkxjV1lB5HFBe3UQ0d3nPgcnPu"
#define WIFI_SSID "AMIEian-Pocket"
#define WIFI_PASSWORD "Nitin.1757051"


//Define Firebase Data object
FirebaseData firebaseData;

String path = "/measures";
String jsonStr;

FirebaseJson json;
FirebaseJsonArray jsonArr;

int previous_measure = 0;
int measure = 0;
void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

}

void loop() {
  FirebaseJsonArray jsonArr;

  Serial.println("Collecting measures");
  previous_measure = analogRead(36) >> 4;

  for (int i = 0; i < 500; i++) {
    measure = analogRead(36) >> 4;
    measure = previous_measure/2 + (measure-30)/2;

    jsonArr.add(measure);
    delay(10);
  }
  Serial.println("Forming pack of data");
  json.clear();
  json.set("data", jsonArr);  
  json.set("timestamp", 0);

  
  if (Firebase.pushJSON(firebaseData, "/measures/", json))  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.print("PUSH NAME: ");
    Serial.println(firebaseData.pushName());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  delay(10*60*1000);
}
