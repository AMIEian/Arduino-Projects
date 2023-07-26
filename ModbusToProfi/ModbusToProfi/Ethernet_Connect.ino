bool connectToEth()
{  
  //digitalWrite(WiFi_Error_LED, HIGH);
  Ethernet.init(5);
  Ethernet.begin(mac, ip, myDns);
  Serial.println(Ethernet.localIP());
  /*
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) 
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) 
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) 
      {
        delay(1);
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) 
    {
      Serial.println("Ethernet cable is not connected.");
    }
    Ethernet.begin(mac, ip, myDns);
  } 
  else 
  {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  */
  // give the Ethernet shield a second to initialize:
  delay(1000);
  return true;
}
