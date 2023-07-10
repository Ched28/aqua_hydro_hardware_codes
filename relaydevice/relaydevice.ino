// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA Main Server
#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct mainmsg {
  int type;
  int Relay1;              //type1 rs
  int Relay2;              //type1 rs
  int Relay3;              //type1 rs
  int Relay4;              //type1 rs
  int Status_SD_CARD = 0;  //type2 rs
  int LogIn = 0;           //type2 rs
  int lightVal = 0;        //type3 r
  int temperature = 0;     //type3 r
  int humidity = 0;        //type3 r
  int pHLevel = 0;         //type4 r
  int ECLevel = 0;         //type4 r
  int leak1 = 0;           //type4 r
  int leak2 = 0;           //type3 r
  int bottomleak = 0;      //type3 r
  int swt1 = 0;            //type4 r
  int swt2 = 0;            //type4 r
  int swt3 = 0;            //type4 r
  int swt4 = 0;            //type4 r
  int swt5 = 0;            //type4 r
  int swt6 = 0;            //type3 r
  int swt7 = 0;            //type3 r
  int swt8 = 0;            //type3 r
  int swt9 = 0;            //type3 r
  int swt10 = 0;           //type3 r


} mainmsg;

mainmsg mymainmsg;
mainmsg othermainmsg;
mainmsg tofirebase;
mainmsg fromfirebase;

const int relay1 = 5;  // GPIO5 D1
const int relay2 = 4;  // GPIO4 D2
const int relay3 = 0;  // GPIO0 D3
const int relay4 = 2;  // GPIO2 D4


uint8_t userAddress[] = { 0x48, 0x55, 0x19, 0xC8, 0x05, 0x50 };
uint8_t mainserverAddress[] = { 0x48, 0x55, 0x19, 0xC8, 0x74, 0x1C };


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

    Serial.print("Relay 1: ");
    Serial.println(othermainmsg.Relay1);
    Serial.print("Relay 2: ");
    Serial.println(othermainmsg.Relay2);
    Serial.print("Relay 3: ");
    Serial.println(othermainmsg.Relay3);
    Serial.print("Relay 4: ");
    Serial.println(othermainmsg.Relay4);

  } else {
    Serial.println("No Message Type");
  }
}

constexpr char WIFI_SSID[] = "AndroidAP";

int8_t getWiFiChannel(const char *ssid) {

  if (int8_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }

  return 0;
}

void setup() {
  Serial.begin(115200);
  int8_t channel = getWiFiChannel(WIFI_SSID);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  // Set outputs to HIGH. relay active LOW
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  mymainmsg.Relay1 = 0;
  mymainmsg.Relay2 = 0;
  mymainmsg.Relay3 = 0;
  mymainmsg.Relay4 = 0;

  WiFi.mode(WIFI_STA);

  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(userAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_add_peer(mainserverAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  mymainmsg.type = 1;
  if (othermainmsg.Relay1 != mymainmsg.Relay1){
      mymainmsg.Relay1 = othermainmsg.Relay1;
      Serial.println("Relay1: " + mymainmsg.Relay1);
      if(mymainmsg.Relay1 == 0){
          digitalWrite(relay1,HIGH);
      }else{
           digitalWrite(relay1,LOW);
      }
  }else if(othermainmsg.Relay2 != mymainmsg.Relay2){
     mymainmsg.Relay2 = othermainmsg.Relay2;
     Serial.println("Relay2: " + mymainmsg.Relay2);
      if(mymainmsg.Relay2 == 0){
          digitalWrite(relay2,HIGH);
      }else{
           digitalWrite(relay2,LOW);
      }
  }else if(othermainmsg.Relay3 != mymainmsg.Relay3){
       mymainmsg.Relay3 = othermainmsg.Relay3;
       Serial.println("Relay3: " + mymainmsg.Relay3);
      if(mymainmsg.Relay3 == 0){
          digitalWrite(relay3,HIGH);
      }else{
           digitalWrite(relay3,LOW);
      }
  }else if(othermainmsg.Relay4 != mymainmsg.Relay4){
      mymainmsg.Relay4 = othermainmsg.Relay4;
      Serial.println("Relay4: " + mymainmsg.Relay4);
      if(mymainmsg.Relay4 == 0){
          digitalWrite(relay4,HIGH);
      }else{
          digitalWrite(relay4,LOW);
      }
  }else {
    Serial.println("Other Data Recieved");
  }

  esp_now_send(mainserverAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  esp_now_send(userAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  delay(2000);
}
