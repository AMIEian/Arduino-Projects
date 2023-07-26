//Accelerometer Connections
const int X_Motion = A0;
const int Y_Motion = A1;
const int Z_Motion = A3;

void setup() {
  // put your setup code here, to run once:
  //Configure Accelerometer pins as input
  pinMode(X_Motion,INPUT);
  pinMode(Y_Motion,INPUT);
  pinMode(Z_Motion,INPUT);
  //Serial connection.
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Motion_Detector())
    {
      Serial.println("Movement Detected, Alarm ...!");
    }
   delay(5000);
}

boolean Motion_Detector()
{
  int X_Diff = 0;
  int Y_Diff = 0;
  int Z_Diff = 0;
  int X = 0;
  int Y = 0;
  int Z = 0;
  char readings = 0;
  char counter = 0;
  boolean looper = true;
  while(looper && counter < 5)
    {
      //This loop will take 20 readings for 1 sec and gets accumulated result of each step
      for(readings = 0; readings < 20; readings++)
	{
	  //Get analog readings
	  X = analogRead(X_Motion);
	  Y = analogRead(Y_Motion);
	  Z = analogRead(Z_Motion);
	  //Wait for 50 ms and get new readings and find if there is any change
	  delay(50);
	  X_Diff = X_Diff + (analogRead(X_Motion) - X);
	  Y_Diff = Y_Diff + (analogRead(Y_Motion) - Y);
	  Z_Diff = Z_Diff + (analogRead(Z_Motion) - Z);

          if(X_Diff < 0)
            X_Diff = X_Diff * -1;
          if(Y_Diff < 0)
            Y_Diff = Y_Diff * -1;
          if(Z_Diff < 0)
            Z_Diff = Z_Diff * -1;

          Serial.print(X_Diff);
          Serial.print("\t");
          Serial.print(Y_Diff);
          Serial.print("\t");
          Serial.print(Z_Diff);
          Serial.print("\n");
        }
      if(X_Diff >= 10 || Y_Diff >= 10 || Z_Diff >= 10)
	{
	  looper = true;
          Serial.println("Movement");
	  delay(100);	//Delay for next reading
	  counter++;
          X_Diff = 0;
          Y_Diff = 0;
          Z_Diff = 0;	
	}
      else
        {
	  looper = false;
          Serial.println("No Movement");
        }
    }
  return looper;
}
