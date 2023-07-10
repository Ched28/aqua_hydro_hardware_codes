// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA Sensor2 Server
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <OneWire.h>
#include <DallasTemperature.h>




typedef struct mainmsg {
  int type;
  int Relay1; //type1 
  int Relay2; //type1
  int Relay3; //type1
  int Relay4; //type1
  int Status_SD_CARD; //type2
  int LogIn; //type2
  int lightVal; //type3
  int temperature; //type3
  int humidity; //type3
  int pHLevel; //type3
  int ECLevel; //type4
  int leak1; //type4
  int leak2; //type3
  int buttomleak; //type3
  int swt1; //type5
  int swt2; //type5
  int swt3; //type5
  int swt4; //type5
  int swt5; //type5
  int swt6; //type5
  int swt7; //type5
  int swt8; //type5
  int swt9 ; //type3
  int swt10; //type3
  int waterlevel1;     //type3

} mainmsg;

mainmsg mymainmsg;
mainmsg othermainmsg;
mainmsg tofirebase;
mainmsg fromfirebase;

#define Rain1 D7
#define Rain D4
#define ONE_WIRE_BUS 12



int decimal = 2;  // Decimal places of the sensor value outputs

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

OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

void setup() {
  /* Define analog signal pin as input or receiver from the Multiplexer pin SIG */
  
  int8_t channel = getWiFiChannel(WIFI_SSID);

  
  Serial.begin(115200);
  sensors.begin();

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
  int sensorValue = digitalRead(Rain);
  int sensorValue1 = digitalRead(Rain1);

  sensors.requestTemperatures();
    Serial.println(sensors.getTempCByIndex(0));
  float waterTemp = sensors.getTempCByIndex(0);

  float sensor15 = analogRead(A0) * 4.3 / 1024.0;
  // Serial.println(sensor15);
  float temptemp = 1.0 + 0.02 * (31.5 - 25.0);
  Serial.println(temptemp);
  float ecreal = (sensor15 / temptemp) *  10;

  Serial.println(ecreal);

  mymainmsg.type = 4;
  mymainmsg.ECLevel = ecreal;
  mymainmsg.leak1 = sensorValue;
  mymainmsg.leak2 = sensorValue1;


  esp_now_send(mainserverAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  esp_now_send(userAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  delay(6000);
}
