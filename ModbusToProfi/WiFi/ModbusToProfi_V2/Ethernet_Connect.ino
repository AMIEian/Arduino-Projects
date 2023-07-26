void connectToWiFi()
{
  char idbuff[25];
  id.toCharArray(idbuff, 25);
  char passbuff[25];
  pass.toCharArray(passbuff, 25);
  
  SerialBT.println("Connecting WiFi...");
  //WiFi.mode(WIFI_STA);
  WiFi.begin(idbuff, passbuff);  
  WiFi.config(Local, Gateway, Subnet);
  
  while(WiFi.status() != WL_CONNECTED) 
    {
      SerialBT.print(".");
      delay(500);
    }
}
