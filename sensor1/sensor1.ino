#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <espnow.h>
#include <ArduinoJson.h>
//


// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast   
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//

//sensor1 
// send only


int pinDHT22 = 5;
SimpleDHT22 dht22(pinDHT22);
int lightPen=A0;

uint8_t mainserverAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x02, 0x83};
//uint8_t relayAddress[] = {0xC8, 0xC9, 0xA3, 0x52, 0xFD, 0x78};
uint8_t userAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x05, 0x50};

String recv_jsondata;
String send_jsondata;

StaticJsonDocument<256> doc_to_espnow;  
StaticJsonDocument<256> doc_from_espnow;


typedef struct sensor1_esp_data {
  int type;
  String sensormacaddress1;
  int lightVal;
  byte temperature;
  byte humidity;

} sensor1_esp_data;

sensor1_esp_data mydata;


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(mainserverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(userAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

}

void loop() {
  String macadd = String(WiFi.macAddress());
  int err = SimpleDHTErrSuccess;
  mydata.lightVal = analogRead(lightPen); 
  if ((err = dht22.read(&mydata.temperature, &mydata.humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(2000);
    return;
  }
 Serial.print("Sample OK: ");
  Serial.print((int)mydata.temperature); Serial.print(" *C, ");
  Serial.print((int)mydata.humidity); Serial.println(" %");
  Serial.print((int)mydata.lightVal); Serial.println(" ");
  
   delay(2500);
  mydata.type = 4;
  mydata.sensormacaddress1 = macadd;

  esp_now_send(mainserverAddress, (uint8_t *) &mydata, sizeof(mydata));
  esp_now_send(userAddress, (uint8_t *) &mydata, sizeof(mydata));
}
