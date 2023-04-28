// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast   
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA HARDWARE

#include <ESP8266WiFi.h>
#include <espnow.h>




uint8_t mainserverAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x02, 0x83};
uint8_t relayAddress[] = {0xC8, 0xC9, 0xA3, 0x52, 0xFD, 0x78};
String lightVal;
String temperature;
String humidity;

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

relay_esp_data relaydata;
relay_esp_data Incomingrelaydata;


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
 //Serial.print("Size:");
  //Serial.println(len);
  int type; 
  memcpy(&type, incomingData, sizeof(int));
  //Serial.print("Type:");
  //Serial.println(type);
  if(type==2){
      memcpy(&Incomingrelaydata, incomingData, sizeof(Incomingrelaydata));
    if(Incomingrelaydata.Relay1 != relaydata.Relay1){
        relaydata.Relay1 = Incomingrelaydata.Relay1;
      
    }
    if(Incomingrelaydata.Relay2 != relaydata.Relay2){
        relaydata.Relay2 = Incomingrelaydata.Relay2;
        
    }
    if(Incomingrelaydata.Relay3 != relaydata.Relay3){
        relaydata.Relay3 = Incomingrelaydata.Relay3;
        
    }
    if(Incomingrelaydata.Relay4 != relaydata.Relay4){
        relaydata.Relay4 = Incomingrelaydata.Relay4;
      
    } 
    return;

    }
    else if(type==4){
       memcpy(&incomingdatasensor1, incomingData, sizeof(incomingdatasensor1));

      
      
       temperature = String(incomingdatasensor1.temperature).c_str();
       lightVal = String(incomingdatasensor1.lightVal).c_str();
       humidity = String(incomingdatasensor1.humidity).c_str();

        //Serial.println(temperature);
        //Serial.println(lightVal);
        //Serial.println(humidity);
     
        
   
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
  
  esp_now_add_peer(mainserverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(relayAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  esp_now_register_recv_cb(OnDataRecv);
  


}

void loop() {


}


void page4(){
           String st1 = String(temperature + " C");
            String st2 = String(humidity + " %");
          Serial.print("t2.txt=\"" +  String(st1));
          Serial.print("\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
          
          Serial.print("t4.txt=\"" +  String(st2));
          Serial.print("\"");
           
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);

}
