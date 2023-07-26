/**
 * @file player-spiffs-i2s.ino
 * @brief example using the SPIFFS library
 * 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include "AudioTools.h"
#include "AudioLibs/AudioSourceSPIFFS.h"
#include "AudioCodecs/CodecMP3Helix.h"

const char *startFilePath="/";
const char* ext="mp3";
AudioSourceSPIFFS source(startFilePath, ext);
I2SStream i2sStream;
MP3DecoderHelix decoder;
AudioPlayer player(source, i2sStream, decoder);

void printMetaData(MetaDataType type, const char* str, int len){
  Serial.print("==> ");
  Serial.print(toStr(type));
  Serial.print(": ");
  Serial.println(str);
}

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  auto config = i2sStream.defaultConfig(TX_MODE);
  config.pin_bck = 15;
  config.pin_ws = 14;
  config.pin_data = 22;
  config.i2s_format = I2S_STD_FORMAT;
  i2sStream.begin(config);

  // setup player
  //source.setFileFilter("*Bob Dylan*");
  player.setMetadataCallback(printMetaData);
  player.begin();
}

void loop() {
  player.copy();
}
