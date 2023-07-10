// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA Main Server
#include <ESP8266WiFi.h>
#include <espnow.h>



typedef struct mainmsg {
  int type;
  int Relay1;          //type1 rs
  int Relay2;          //type1 rs
  int Relay3;          //type1 rs
  int Relay4;          //type1 rs
  int Status_SD_CARD;  //type2 rs
  int LogIn;           //type2 rs
  int lightVal;        //type3 r
  int temperature;     //type3 r
  int humidity;        //type3 r
  float pHLevel;       //type4 r
  float ECLevel;       //type4 r
  int leak1;           //type4 r
  int leak2;           //type4 r
  int bottomleak;      //type3 r
  int swt1;            //type4 r
  int swt2;            //type4 r
  int swt3;            //type4 r
  int swt4;            //type4 r
  int swt5;            //type4 r
  int swt6;            //type3 r
  int swt7;            //type3 r
  int swt8;            //type3 r
  int swt9;            //type3 r
  int swt10;           //type3 r

  int waterlevel1;  //type3

} mainmsg;

mainmsg mymainmsg;
mainmsg othermainmsg;
mainmsg tofirebase;
mainmsg fromfirebase;






uint8_t relayAddress[] = { 0xC8, 0xC9, 0xA3, 0x54, 0xF7, 0xAA };
uint8_t mainserverAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x74, 0x1C};


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char address[18];
  Serial.print("Sent to: ");
  snprintf(address, sizeof(address), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(address);
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {

  char macStr[18];
  Serial.print("Received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&othermainmsg, incomingData, sizeof(othermainmsg));
  Serial.printf("Board %u: %u bytes\n", othermainmsg.type, len);
  if (othermainmsg.type == 1) {
    mymainmsg.Relay1 = othermainmsg.Relay1;
    Serial.print("Relay 1: ");
    Serial.println(mymainmsg.Relay1);

    mymainmsg.Relay2 = othermainmsg.Relay2;
    Serial.print("Relay 2: ");
    Serial.println(mymainmsg.Relay2);
    mymainmsg.Relay3 = othermainmsg.Relay3;
    Serial.print("Relay 3: ");
    Serial.println(mymainmsg.Relay3);
    mymainmsg.Relay4 = othermainmsg.Relay4;
    Serial.print("Relay 4: ");
    Serial.println(mymainmsg.Relay4);

  } else if (othermainmsg.type == 2) {

    mymainmsg.Status_SD_CARD = othermainmsg.Status_SD_CARD;
    Serial.print("Status SD Card: ");
    Serial.println(mymainmsg.Status_SD_CARD);
    mymainmsg.LogIn = othermainmsg.LogIn;
    Serial.print("Log In: ");
    Serial.println(mymainmsg.LogIn);
  } else if (othermainmsg.type == 3) {

    mymainmsg.lightVal = othermainmsg.lightVal;
    Serial.print("Light Value: ");
    Serial.println(mymainmsg.lightVal);
    mymainmsg.temperature = othermainmsg.temperature;
    Serial.print("Temperature: ");
    Serial.println(mymainmsg.temperature);
    mymainmsg.humidity = othermainmsg.humidity;
    Serial.print("Humidity: ");
    Serial.println(mymainmsg.humidity);

    mymainmsg.bottomleak = othermainmsg.bottomleak;
    Serial.print("Bottom Leak: ");
    Serial.println(mymainmsg.bottomleak);
    mymainmsg.swt6 = othermainmsg.swt6;
    Serial.print("Container 6: ");
    Serial.println(mymainmsg.swt6);
    mymainmsg.swt7 = othermainmsg.swt7;
    Serial.print("Container 7: ");
    Serial.println(mymainmsg.swt7);
    mymainmsg.swt8 = othermainmsg.swt8;
    Serial.print("Container 8: ");
    Serial.println(mymainmsg.swt8);
    mymainmsg.swt9 = othermainmsg.swt9;
    Serial.print("Container 9: ");
    Serial.println(mymainmsg.swt9);
    mymainmsg.swt10 = othermainmsg.swt10;
    Serial.print("Container 10: ");
    Serial.println(mymainmsg.swt10);
  } else if (othermainmsg.type == 4) {
    mymainmsg.pHLevel = othermainmsg.pHLevel;
    Serial.print("Ph Level: ");
    Serial.println(mymainmsg.pHLevel);
    mymainmsg.ECLevel = othermainmsg.ECLevel;
    Serial.print("EC Level: ");
    Serial.println(mymainmsg.ECLevel);
    mymainmsg.leak1 = othermainmsg.leak1;
    Serial.print("Leak 1: ");
    Serial.println(mymainmsg.leak1);
    mymainmsg.leak2 = othermainmsg.leak2;
    Serial.print("Leak 2: ");
    Serial.println(mymainmsg.leak2);
    mymainmsg.swt1 = othermainmsg.swt1;
    Serial.print("Container 1: ");
    Serial.println(mymainmsg.swt1);
    mymainmsg.swt2 = othermainmsg.swt2;
    Serial.print("Container 2: ");
    Serial.println(mymainmsg.swt2);
    mymainmsg.swt3 = othermainmsg.swt3;
    Serial.print("Container 3: ");
    Serial.println(mymainmsg.swt3);
    mymainmsg.swt4 = othermainmsg.swt4;
    Serial.print("Container 4: ");
    Serial.println(mymainmsg.swt4);
    mymainmsg.swt5 = othermainmsg.swt5;
    Serial.print("Container 5: ");
    Serial.println(mymainmsg.swt5);

  } else {
    Serial.println("No Message Type");
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
  esp_now_add_peer(mainserverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  mymainmsg.type = 1;
  mymainmsg.Relay1 = 1;
  mymainmsg.Relay2 = 1;
  mymainmsg.Relay3 = 1;
  mymainmsg.Relay4 = 0;
  esp_now_send(relayAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  esp_now_send(mainserverAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  delay(2000);
}
