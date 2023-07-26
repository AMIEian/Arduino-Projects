bool connectToWiFi()
{
  char idbuff[25];
  id.toCharArray(idbuff, 25);
  char passbuff[25];
  pass.toCharArray(passbuff, 25);
  int waitCount = 0;
  
  SerialBT.println("Connecting WiFi...");
  digitalWrite(WiFi_Error_LED, HIGH);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(idbuff, passbuff);  
  WiFi.config(Local, Gateway, Subnet);
  
  while(WiFi.status() != WL_CONNECTED && waitCount < 60) 
    {
      digitalWrite(WiFi_Error_LED, ~digitalRead (WiFi_Error_LED));
      SerialBT.print(".");
      delay(500);
      waitCount++;
    }
  if(waitCount < 60)
    {
      //Connected to WiFi within 10 secs.
      digitalWrite(WiFi_Error_LED, LOW);
      return true;
    }
  else
    {
      //Connected to WiFi within 10 secs.
      digitalWrite(WiFi_Error_LED, HIGH);
      return false;
    }
}

bool connectToEth()
{  
  //digitalWrite(WiFi_Error_LED, HIGH);
  Ethernet.init(5);
  Ethernet.begin(mac, Local, Gateway);
  SerialBT.println(Ethernet.localIP());
  delay(1000);
  return true;
}
