// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast   
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA Main Server
#include <ESP8266WiFi.h>
#include <espnow.h>


typedef struct main_esp_data {
  int type;
  String fixedmacaddress;
  String WifiName;
  String IPAddress;

} main_esp_data;

main_esp_data mydata;
main_esp_data myIncomingData;


typedef struct relay_esp_data {
  int type;
  String relaymacaddress;
  int Relay1;
  int Relay2;
  int Relay3;
  int Relay4;

} relay_esp_data;

relay_esp_data myrelaydata;
relay_esp_data myIncomingrelaydata;


typedef struct user_esp_data {
  int type;
  String usermacaddress;
  int Status_SD_CARD;
  int LogIn;
  String RFID_DATA;

} user_esp_data;

user_esp_data userdata;
user_esp_data Incominguserdata;



typedef struct sensor1_esp_data {
  int type;
  String sensormacaddress1;
  int lightVal;
  byte temperature;
  byte humidity;

} sensor1_esp_data;

sensor1_esp_data incomingdatasensor1;


uint8_t relayAddress[] = {0xC8, 0xC9, 0xA3, 0x52, 0xFD, 0x78};
uint8_t userAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x05, 0x50};
uint8_t WiFiAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x74, 0x1C};
//uint8_t mainserverAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x02, 0x83};


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  Serial.print("Size:");
  Serial.println(len);
  int type; 
  memcpy(&type, incomingData, sizeof(int));
  Serial.print("Type:");
  Serial.println(type);
  if(type==2){
      memcpy(&myIncomingrelaydata, incomingData, sizeof(myIncomingrelaydata));
    if(myIncomingrelaydata.Relay1 != myrelaydata.Relay1){
        myrelaydata.Relay1 = myIncomingrelaydata.Relay1;
      
    }
    if(myIncomingrelaydata.Relay2 != myrelaydata.Relay2){
        myrelaydata.Relay2 = myIncomingrelaydata.Relay2;
        
    }
    if(myIncomingrelaydata.Relay3 != myrelaydata.Relay3){
        myrelaydata.Relay3 = myIncomingrelaydata.Relay3;
        
    }
    if(myIncomingrelaydata.Relay4 != myrelaydata.Relay4){
        myrelaydata.Relay4 = myIncomingrelaydata.Relay4;
      
    }

    Serial.write(incomingData, len);
    Serial.write('\n');
  

    

    
  } else if(type==4){
       memcpy(&incomingdatasensor1, incomingData, sizeof(incomingdatasensor1));

      
      
      String temperature = String(incomingdatasensor1.temperature).c_str();
       String lightVal = String(incomingdatasensor1.lightVal).c_str();
       String humidity = String(incomingdatasensor1.humidity).c_str();

       Serial.println(temperature);
       Serial.println(lightVal);
       Serial.println(humidity);
   
    }



}

void setup() {
  Serial.begin(115200);
    WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(relayAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(userAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
}
