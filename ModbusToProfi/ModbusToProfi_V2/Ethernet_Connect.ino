bool connectToEth()
{  
  //digitalWrite(WiFi_Error_LED, HIGH);
  ethReset();
  Ethernet.init(5);
  Ethernet.begin(mac, ip, myDns);
  SerialBT.println(Ethernet.localIP());
  // give the Ethernet shield a second to initialize:
  delay(1000);
  return true;
}

void ethReset()
{
  digitalWrite(WizReset, LOW);
  delay(1000);
  digitalWrite(WizReset, HIGH);
  delay(1000);
}
