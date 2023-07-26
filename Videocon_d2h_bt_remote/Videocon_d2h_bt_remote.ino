#include <IRremote.h>

IRsend irsend;
char inChar;
char command;
void setup() 
  {
    Serial.begin(9600);
  }

void loop() 
  {
    if(command == '0')
      {
        irsend.sendNEC(0x6F900FF, 32);
        command = 'X';
      }
    else if(command == '1')
      {
        irsend.sendNEC(0x6F9807F, 32);
        command = 'X';
      }
    else if(command == '2')
      {
        irsend.sendNEC(0x6F940BF, 32);
        command = 'X';
      }
    else if(command == '3')
      {
        irsend.sendNEC(0x6F9C03F, 32);
        command = 'X';
      }
    else if(command == '4')
      {
        irsend.sendNEC(0x6F920DF, 32);
        command = 'X';
      }
    else if(command == '5')
      {
        irsend.sendNEC(0x6F9A05F, 32);
        command = 'X';
      }
    else if(command == '6')
      {
        irsend.sendNEC(0x6F9609F, 32);
        command = 'X';
      }
    else if(command == '7')
      {
        irsend.sendNEC(0x6F9E01F, 32);
        command = 'X';
      }
    else if(command == '8')
      {
        irsend.sendNEC(0x6F910EF, 32);
        command = 'X';
      }
    else if(command == '9')
      {
        irsend.sendNEC(0x6F9906F, 32);
        command = 'X';
      }
    else if(command == 'A')  //Vol. Up
      {
        irsend.sendNEC(0x6F958A7, 32);
        command = 'X';
      }
    else if(command == 'B')  //Vol. Dw
      {
        irsend.sendNEC(0x6F97887, 32);
        command = 'X';
      }
    else if(command == 'C')  //Ch +
      {
        irsend.sendNEC(0x6F9D827, 32);
        command = 'X';
      }
    else if(command == 'D')  //Ch -
      {
        irsend.sendNEC(0x6F9F807, 32);
        command = 'X';
      }
    else if(command == 'E')  //Stb Off
      {
        irsend.sendNEC(0x6F948B7, 32);
        command = 'X';
      }
    else if(command == 'F')  //Back
      {
        irsend.sendNEC(0x6F9708F, 32);
        command = 'X';
      }
    else if(command == 'G')  //Exit
      {
        irsend.sendNEC(0x6F9DA25, 32);
        command = 'X';
      }
    else if(command == 'H')  //Ok
      {
        irsend.sendNEC(0x6F906F9, 32);
        command = 'X';
      }
    else if(command == 'I')  //Info
      {
        irsend.sendNEC(0x6F94AB5, 32);
        command = 'X';
      }
    else if(command == 'J')  //Mute
      {
        irsend.sendNEC(0x6F908F7, 32);
        command = 'X';
      }
    else if(command == 'K')  //Up
      {
        irsend.sendNEC(0x6F9FA05, 32);
        command = 'X';
      }
    else if(command == 'L')  //Down
      {
        irsend.sendNEC(0x6F92AD5, 32);
        command = 'X';
      }
    else if(command == 'M')  //Left
      {
        irsend.sendNEC(0x6F99A65, 32);
        command = 'X';
      }
    else if(command == 'N')  //Right
      {
        irsend.sendNEC(0x6F91AE5, 32);
        command = 'X';
      }
    else if(command == 'O')  //Repeat
      {
        irsend.sendNEC(0xFFFFFFFF, 32);
        command = 'X';
      }
  }
void serialEvent() 
  {
    while (Serial.available()) 
      {
        inChar = (char)Serial.read();
        if(inChar >= '0' && inChar <= 'Z')
          command = inChar;
      }
  }
