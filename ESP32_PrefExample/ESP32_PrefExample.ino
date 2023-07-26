/*
This example shows how to use Preferences (nvs) to store a
structure.  Note that the maximum size of a putBytes is 496K
or 97% of the nvs partition size.  nvs has signifcant overhead,
so should not be used for data that will change often.
*/ 
#include <Preferences.h>
Preferences prefs;

void setup() {
  Serial.begin(9600);
  
  int16_t zero[20] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
  int16_t ones[20];
  prefs.begin("schedule2"); // use "schedule" namespace

  prefs.getBytes("test", ones, sizeof(ones));
  
  for (int x=0; x<20; x++)
    Serial.println(ones[x]);
  Serial.println(); 
  
  prefs.putBytes("test", zero, sizeof(zero));

  for (int x=0; x<20; x++)
    zero[x] = 99;

  for (int x=0; x<20; x++)
    Serial.println(zero[x]);
  Serial.println(); 
  
  prefs.getBytes("test", zero, sizeof(zero));
  
  for (int x=0; x<20; x++)
    Serial.println(zero[x]);
  Serial.println(); 

  prefs.clear();

  prefs.getBytes("test", ones, sizeof(ones));
  
  for (int x=0; x<20; x++)
    Serial.println(ones[x]);
  Serial.println(); 
}

void loop() {}
