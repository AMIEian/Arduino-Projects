// VT82885P ADDRESS REGISTERS
const uint8_t REGISTER_SECONDS = B00000000; // Seconds
const uint8_t REGISTER_SECONDS_ALARM = B00000001; // Seconds alarm
const uint8_t REGISTER_MINUTES = B00000010; // Minutes
const uint8_t REGISTER_MINUTES_ALARM = B00000011; // Minutes alarm
const uint8_t REGISTER_HOURS = B00000100; // Hours
const uint8_t REGISTER_HOURS_ALARM = B00000101; // Hours alarm
const uint8_t REGISTER_DOW = B00000110; // Day of week
const uint8_t REGISTER_DOM = B00000111; // Day of month
const uint8_t REGISTER_MONTH = B00001000; // Month
const uint8_t REGISTER_YEAR = B00001001; // Year
const uint8_t REGISTER_A = B00001010; // REGISTER A
const uint8_t REGISTER_B = B00001011; // REGISTER B
const uint8_t REGISTER_C = B00001100; // REGISTER C
const uint8_t REGISTER_D = B00001101; // REGISTER D

// dataPins = AD0 - AD7
int dataPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};
// int dataPins[8] = {9, 8, 7, 6, 5, 4, 3, 2}; INVERTED
int CS = A4; // SELECT CHIP
int AS = A3; // STROBE ADDRESS
int WR = A2; // HIGH READ LOW WRITE
int DS = A1; // READ

uint8_t seconds;
uint8_t asecs;
uint8_t minutes;
uint8_t amins;
uint8_t hours;
uint8_t ahour;
uint8_t dow;
uint8_t dom;
uint8_t month;
uint8_t year;
uint8_t regA;
uint8_t rebB;
uint8_t rebC;
uint8_t rebD;
uint8_t lastsecs = 0b11111111;

boolean getBit(uint8_t myVarIn, uint8_t whatBit) {
boolean bitState;
bitState = myVarIn & (1 << whatBit); return bitState; } uint8_t setBit(uint8_t myVarIn, uint8_t whatBit, boolean bitState) { if (bitState) { myVarIn |= (1 << whatBit); } else { myVarIn &= ~(1 << whatBit); } return myVarIn; } void setIdle() { digitalWrite(CS, HIGH); digitalWrite(AS, HIGH); digitalWrite(WR, HIGH); digitalWrite(DS, HIGH); } void switchRead() { for (int i=0; i<7; i++) { pinMode(dataPins[i], INPUT); } } void switchWrite() { for (int i=0; i<7; i++) { pinMode(dataPins[i], OUTPUT); } } uint8_t readBusData() { uint8_t dataBuffer = 0b00000000; for (int i=0; i<8; i++) { boolean state = digitalRead(dataPins[i]); dataBuffer = setBit(dataBuffer, i, state); } return dataBuffer; } void writeBusData(uint8_t data) { for (int i=0; i<8; i++) { digitalWrite(dataPins[i], getBit(data, i)); } } void readSeconds() { seconds = getRegisterData(REGISTER_SECONDS); } uint8_t getRegisterData(uint8_t registerAddr) { uint8_t dataBuffer = 0b00000000; switchWrite(); digitalWrite(CS, LOW); delayMicroseconds(20); writeBusData(registerAddr); digitalWrite(AS, LOW); delayMicroseconds(20); switchRead(); delayMicroseconds(20); digitalWrite(DS, LOW); delayMicroseconds(20); dataBuffer = readBusData(); digitalWrite(DS, HIGH); delayMicroseconds(20); digitalWrite(AS, HIGH); delayMicroseconds(20); digitalWrite(CS, HIGH); return dataBuffer; } void getRTCDate() { seconds = getRegisterData(REGISTER_SECONDS); minutes = getRegisterData(REGISTER_MINUTES); hours = getRegisterData(REGISTER_HOURS); dow = getRegisterData(REGISTER_DOW); dom = getRegisterData(REGISTER_DOM); month = getRegisterData(REGISTER_MONTH); year = getRegisterData(REGISTER_YEAR); } void setup() { pinMode(CS, OUTPUT); pinMode(AS, OUTPUT); pinMode(WR, OUTPUT); pinMode(DS, OUTPUT); setIdle(); switchRead(); Serial.begin(9600); } void loop() { getRTCDate(); Serial.print("("); Serial.print(dow); Serial.print(") "); Serial.print(month); Serial.print("/"); Serial.print(dom); Serial.print("/"); Serial.print(year); Serial.print(" "); Serial.print(hours); Serial.print(":"); Serial.print(minutes); Serial.print("."); Serial.println(seconds); delay(1000); /* if (lastsecs != seconds) { lastsecs = seconds; for (int i=0; i<8; i++) { //Serial.print(getBit(seconds, i)); } } */ }
