void writeStringToEEPROM(uint8_t addrOffset, const String &strToWrite)
{
  uint8_t len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (uint8_t i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(uint8_t addrOffset)
{
  uint8_t newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];

  for (uint8_t i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}
