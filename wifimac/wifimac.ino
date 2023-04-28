#include <ESP8266WiFi.h>


void setup() {
   Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  String macadd = String(WiFi.macAddress());
  const char* c = macadd.c_str();
  Serial.println(c);
  String s = "";
  s = c;
  Serial.println(s);
}

void loop() {
  // put your main code here, to run repeatedly:

}
