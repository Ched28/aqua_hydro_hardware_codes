// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//


// AQUA Main Server
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define APP_KEY "AIzaSyAXIYk1apQQ0M1a57TWNsx2_nVqRvXD3rw"
#define FIREBASE_PROJECT_ID "aqua-monitoring-system-c7b63"
#define USER_EMAIL "chedrick.follero.rowy@gmail.com"
#define USER_PASSWORD "YnKak4qw"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


const char *ssid = "AndroidAP";
const char *password = "ched12345";

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

  int swt1;   //type4 r
  int swt2;   //type4 r
  int swt3;   //type4 r
  int swt4;   //type4 r
  int swt5;   //type4 r
  int swt6;   //type3 r
  int swt7;   //type3 r
  int swt8;   //type3 r
  int swt9;   //type3 r
  int swt10;  //type3 r

  int waterlevel1;  //type3

} mainmsg;

mainmsg mymainmsg;
mainmsg othermainmsg;
mainmsg tofirebase;
mainmsg fromfirebase;
mainmsg tempfile;





uint8_t relayAddress[] = { 0xC8, 0xC9, 0xA3, 0x54, 0xF7, 0xAA };
uint8_t userAddress[] = { 0x48, 0x55, 0x19, 0xC8, 0x05, 0x50 };
uint8_t sensor1[] = { 0x48, 0x55, 0x19, 0xC8, 0x63, 0x61 };
//uint8_t mainserverAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x02, 0x83};


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
    mymainmsg.pHLevel = othermainmsg.pHLevel;
    Serial.print("Ph Level: ");
    Serial.println(mymainmsg.pHLevel);



  } else if (othermainmsg.type == 4) {

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


  } else if (othermainmsg.type == 5) {

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
    mymainmsg.swt6 = othermainmsg.swt6;
    Serial.print("Container 6: ");
    Serial.println(mymainmsg.swt6);
    mymainmsg.swt7 = othermainmsg.swt7;
    Serial.print("Container 7: ");
    Serial.println(mymainmsg.swt7);
    mymainmsg.swt8 = othermainmsg.swt8;
    Serial.print("Container 8: ");
    Serial.println(mymainmsg.swt8);
    Serial.print("Container 1: ");
    Serial.println(mymainmsg.swt1);
    mymainmsg.swt9 = othermainmsg.swt4;
    Serial.print("Container 4: ");
    Serial.println(mymainmsg.swt4);
    mymainmsg.swt10 = othermainmsg.swt10;
    Serial.print("Container 10: ");
    Serial.println(mymainmsg.swt10);
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


WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset

NTPClient timeClient(ntpUDP, "ph.pool.ntp.org", 28800, 60000);

//Week Days
String weekDays[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

//Month names
String months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };



void setup() {
  Serial.begin(115200);

  int8_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.mode(WIFI_AP_STA);


  WiFi.begin(ssid, password);


  Serial.printf("Connecting to %s .", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println(" ok");

  IPAddress ip = WiFi.localIP();


  // WiFi.printDiag(Serial);
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);

  // WiFi.printDiag(Serial);
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
  config.api_key = APP_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(relayAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_add_peer(userAddress, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_add_peer(sensor1, ESP_NOW_ROLE_COMBO, channel, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  //timestamp
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();

  String formattedTime = timeClient.getFormattedTime();
  //Serial.print("Formatted Time: ");
  //Serial.println(formattedTime);
  int currentHour = timeClient.getHours();
  // Serial.print("Hour: ");
  // Serial.println(currentHour);

  int currentMinute = timeClient.getMinutes();
  // Serial.print("Minutes: ");
  // Serial.println(currentMinute);

  int currentSecond = timeClient.getSeconds();
  //Serial.print("Seconds: ");
  // Serial.println(currentSecond);

  String weekDay = weekDays[timeClient.getDay()];
  // Serial.print("Week Day: ");
  // Serial.println(weekDay);

  struct tm *ptm = gmtime((time_t *)&epochTime);

  int monthDay = ptm->tm_mday;
  //Serial.print("Month day: ");
  // Serial.println(monthDay);

  int currentMonth = ptm->tm_mon + 1;
  //Serial.print("Month: ");
  //Serial.println(currentMonth);

  String currentMonthName = months[currentMonth - 1];
  //Serial.print("Month name: ");
  // Serial.println(currentMonthName);

  int currentYear = ptm->tm_year + 1900;
  // Serial.print("Year: ");
  // Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  //Serial.print("Current date: ");
  //Serial.println(currentDate);

  //Serial.println("");



  // Serial.println("=================================");
  //Serial.println("Data");
  String datetime1 = currentDate + "T" + formattedTime + "Z";
  Serial.println(datetime1);

  mymainmsg.type = 1;
  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {

    String documentPath1 = "scheduler/air_pump";
    String mask1 = "switch";
    String documentPath2 = "scheduler/cooling_fan";
    String mask2 = "switch";
    String documentPath3 = "scheduler/grow_light";
    String mask3 = "switch";
    String mask = "night_switch";
    String documentPath4 = "scheduler/water_pump";
    String mask4 = "switch";
    bool relayswitch1;
    bool relayswitch2;
    bool relayswitch3;
    bool relayswitch4;

    FirebaseJson result;
    FirebaseJsonData result1;

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath1.c_str(), mask1.c_str())) {

      String paaay = String(fbdo.payload().c_str());
      StaticJsonDocument<256> doc_to_espnow;
      deserializeJson(doc_to_espnow, paaay);
      JsonObject obj = doc_to_espnow.as<JsonObject>();

      String result = obj["fields"]["switch"]["booleanValue"];

      Serial.println(result);
      if (result == "false") {
        mymainmsg.Relay1 = 0;


      } else {
        mymainmsg.Relay1 = 1;
      }
    }
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath2.c_str(), mask2.c_str())) {

      String paaay = String(fbdo.payload().c_str());
      StaticJsonDocument<256> doc_to_espnow;


      deserializeJson(doc_to_espnow, paaay);
      JsonObject obj = doc_to_espnow.as<JsonObject>();

      String result = obj["fields"]["switch"]["booleanValue"];

      Serial.println(result);

      if (result == "false") {
        mymainmsg.Relay2 = 0;


      } else {
        mymainmsg.Relay2 = 1;
      }
    }
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath3.c_str(), mask3.c_str())) {

      String paaay = String(fbdo.payload().c_str());
      StaticJsonDocument<256> doc_to_espnow;


      deserializeJson(doc_to_espnow, paaay);
      JsonObject obj = doc_to_espnow.as<JsonObject>();

      String result = obj["fields"]["switch"]["booleanValue"];

      Serial.println(result);

      if (result == "false") {
        mymainmsg.Relay3 = 0;


      } else {
        mymainmsg.Relay3 = 1;
      }
    }

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath4.c_str(), mask4.c_str())) {

      String paaay = String(fbdo.payload().c_str());
      StaticJsonDocument<256> doc_to_espnow;


      deserializeJson(doc_to_espnow, paaay);
      JsonObject obj = doc_to_espnow.as<JsonObject>();

      String result = obj["fields"]["switch"]["booleanValue"];

      Serial.println(result);

      if (result == "false") {
        mymainmsg.Relay4 = 0;


      } else {
        mymainmsg.Relay4 = 1;
      }
    }

    delay(3000);



    String documentPath5 = "temperature/" + fbdo.to<int>();
    String documentPath6 = "humidity/" + fbdo.to<int>();
    String documentPath7 = "light_resistance/" + fbdo.to<int>();
    String documentPath8 = "ph_level/" + fbdo.to<int>();
    String documentPath9 = "ec_level/" + fbdo.to<int>();
    String documentPath10 = "water_level/" + fbdo.to<int>();

    FirebaseJson content1;
    FirebaseJson content2;
    FirebaseJson content3;
    FirebaseJson content4;
    FirebaseJson content5;
    FirebaseJson content6;

    String temp = String(mymainmsg.temperature);
    String hum = String(mymainmsg.humidity);
    String lss = String(mymainmsg.lightVal);
    String phl = String(mymainmsg.pHLevel);
    String ecl = String(mymainmsg.ECLevel);
    String wtr = String(mymainmsg.leak1);
    content1.set("fields/value/stringValue", temp.c_str());
    content1.set("fields/datetime/timestampValue", datetime1.c_str());
    content2.set("fields/value/stringValue", hum.c_str());
    content2.set("fields/datetime/timestampValue", datetime1.c_str());
    content3.set("fields/value/stringValue", lss.c_str());
    content3.set("fields/datetime/timestampValue", datetime1.c_str());
    content4.set("fields/value/stringValue", phl.c_str());
    content4.set("fields/datetime/timestampValue", datetime1.c_str());
    content5.set("fields/value/stringValue", ecl.c_str());
    content5.set("fields/datetime/timestampValue", datetime1.c_str());
    content6.set("fields/value/stringValue", wtr.c_str());
    content6.set("fields/datetime/timestampValue", datetime1.c_str());
    if (tempfile.temperature != mymainmsg.temperature) {
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath5.c_str(), content1.raw())) {
        Serial.println(fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Data is the Same");
    }

    if (tempfile.humidity != mymainmsg.humidity) {
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath6.c_str(), content2.raw())) {
        Serial.println(fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Data is the same");
    }
    if (tempfile.lightVal != mymainmsg.lightVal) {
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath7.c_str(), content3.raw())) {
        Serial.println(fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Data is the same");
    }
    if (tempfile.pHLevel != mymainmsg.pHLevel) {
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath8.c_str(), content4.raw())) {
        Serial.println(fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Data is the same");
    }
    if (tempfile.ECLevel != tempfile.ECLevel) {
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath9.c_str(), content5.raw())) {
        Serial.println(fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Data is the same");
    }

    if (tempfile.leak1 != mymainmsg.leak1) {

      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath10.c_str(), content6.raw())) {
        Serial.println(fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
    }


  }


  else {
    Serial.println(fbdo.errorReason());
  }

  delay(3000);
  tempfile.temperature = mymainmsg.temperature;
  tempfile.humidity = mymainmsg.humidity;
  tempfile.lightVal = mymainmsg.lightVal;
  tempfile.pHLevel = mymainmsg.pHLevel;
  tempfile.ECLevel = mymainmsg.ECLevel;
  tempfile.leak1 = mymainmsg.leak1;
  tempfile.Relay3 = mymainmsg.Relay3;

  esp_now_send(relayAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  esp_now_send(userAddress, (uint8_t *)&mymainmsg, sizeof(mymainmsg));
  delay(2000);
}
