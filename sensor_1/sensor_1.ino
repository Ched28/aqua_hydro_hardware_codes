// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA Main Server
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SimpleDHT.h>

int pinDHT22 = 5;
SimpleDHT22 dht22(pinDHT22);
int lightPen = 4;

typedef struct mainmsg {
  int type;
  int Relay1 = 0;          //type1 rs
  int Relay2 = 0;          //type1 rs
  int Relay3 = 0;          //type1 rs
  int Relay4 = 0;          //type1 rs
  int Status_SD_CARD = 0;  //type2 rs
  int LogIn = 0;           //type2 rs
  int lightVal;            //type3 r
  int temperature;         //type3 r
  int humidity;            //type3 r
  float pHLevel;       //type4 r
  float ECLevel;       //type4 r
  int leak1 = 0;           //type4 r
  int leak2 = 0;           //type4 r
  int swt1 = 0;            //type4 r
  int swt2 = 0;            //type4 r
  int swt3 = 0;            //type4 r
  int swt4 = 0;            //type4 r
  int swt5 = 0;            //type4 r
  int swt6;                //type3 r
  int swt7;                //type3 r
  int swt8;                //type3 r
  int swt9;                //type3 r
  int swt10;               //type3 r


} mainmsg;

mainmsg mymainmsg;
mainmsg othermainmsg;
mainmsg tofirebase;
mainmsg fromfirebase;






uint8_t relayAddress[] = { 0xC8, 0xC9, 0xA3, 0x52, 0xFD, 0x78 };
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


byte temperature;
byte humidity;
int lightVal;
int buf[10], temp;
unsigned long int avgValue;

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

  WiFi.mode(WIFI_STA);

  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(relayAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_add_peer(userAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_add_peer(mainserverAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
}

void loop() {

  
  for (int i = 0; i < 10; i++)  //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(A0);
    delay(10);
  }
  for (int i = 0; i < 9; i++)  //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)  //take the average value of 6 center sample
  avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6;  //convert the analog into millivolt
  phValue = 3.5 * phValue;
  //Serial.println(phValue);
  int err = SimpleDHTErrSuccess;
  lightVal = digitalRead(lightPen);
  if ((err = dht22.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    delay(4000);
    return;
  }
  mymainmsg.type = 3;
  mymainmsg.lightVal = lightVal;
  mymainmsg.temperature = (int)temperature;
  mymainmsg.humidity = (int)humidity;
 
  mymainmsg.pHLevel = phValue;
  esp_now_send(userAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  esp_now_send(mainserverAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  delay(4000);
}
