// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


typedef struct struct_message1 {
  int lightVal;
  byte temperature = 0;
  byte humidity = 0;

} struct_message1;
struct_message1 SENDER1;



LiquidCrystal_I2C lcd(0x27, 20, 4);


void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&SENDER1, incomingData, sizeof(SENDER1));
  lcd.begin();
  lcd.clear();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("AQUA APPLICATION");
  lcd.setCursor(0, 1);
  lcd.print("tempature  "); lcd.print((int)SENDER1.temperature); lcd.print(" C "); 
  Serial.println(SENDER1.temperature);
  lcd.setCursor(0, 2);
  lcd.print("humidity  ");
  lcd.print((int)SENDER1.humidity);
  lcd.print(" %");
  lcd.setCursor(0, 3);
  lcd.print("LDR:  ");
  lcd.print((int)SENDER1.lightVal);
  lcd.print(" ");
  
  delay(1000);


 
  
}



void setup(){
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

 

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);

  
}
 
void loop(){


}