#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <espnow.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define WIFI_SSID "ched_wifi"
//WiFi SSID to which you want NodeMCU to connect
#define WIFI_PASSWORD "YnKak4qw"

#define APP_KEY "AIzaSyDUhgOat1FWBHFj1Hl1GqYLFkuDxFPW-hY"
#define FIREBASE_PROJECT_ID "aqua-monitoring-system"
#define USER_EMAIL "chedrick.follero.rowy@gmail.com"
#define USER_PASSWORD "YnKak4qw"


//esp_now_add_peer(WiFiAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
uint8_t Universal[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

String recv_jsondata;
String send_jsondata;

StaticJsonDocument<256> doc_send;
StaticJsonDocument<256> doc_recv;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset

NTPClient timeClient(ntpUDP, "ph.pool.ntp.org", 28800, 60000);

//Week Days
String weekDays[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

//Month names
String months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };


String temp;
String hum;
String lss;
String R1;
String R2;
String R3;
String R4;
int sendr1;
int sendr2;
int sendr3;
int sendr4;
String type1;
 void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  char *buff = (char *)incomingData;
  recv_jsondata = String(buff);
  Serial.print("Recieved ");
  Serial.println(recv_jsondata);
  DeserializationError error = deserializeJson(doc_recv, recv_jsondata);

  if (!error) {
    type1 = String(doc_recv["type"]);
    int type2 = type1.toInt();
    if (type2 == 4) {
      temp = String(doc_recv["temperature"]);
      hum = String(doc_recv["humidity"]);
      lss = String(doc_recv["lightVal"]);
    } else if (type2 == 2) {
      R1 = String(doc_recv["relay1"]);
      R2 = String(doc_recv["relay2"]);
      R3 = String(doc_recv["relay3"]);
      R4 = String(doc_recv["relay4"]);
    }


  } else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  recv_jsondata = "";
}



void setup() {

  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }


  timeClient.begin();
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());  //print local IP address
  Serial.println(WiFi.channel());
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


  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  //timestamp
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();

  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);
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
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");



  Serial.println("=================================");
  Serial.println("Data");
  String datetime1 = currentDate + "T" + formattedTime + "Z";
  Serial.println(datetime1);


  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {




    int type = type1.toInt();
    // Save firebase
    if (type == 4) {
      String documentPath1 = "temperature/" + fbdo.to<int>();
      String documentPath2 = "humidity/" + fbdo.to<int>();
      String documentPath3 = "light_resistance/" + fbdo.to<int>();
      FirebaseJson content1;
      FirebaseJson content2;
      FirebaseJson content3;


      content1.set("fields/value/stringValue", temp.c_str());
      content1.set("fields/datetime/timestampValue", datetime1.c_str());
      content2.set("fields/value/stringValue", hum.c_str());
      content2.set("fields/datetime/timestampValue", datetime1.c_str());
      content3.set("fields/value/stringValue", lss.c_str());
      content3.set("fields/datetime/timestampValue", datetime1.c_str());

      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath1.c_str(), content1.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath2.c_str(), content2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());

      } else {
        Serial.println(fbdo.errorReason());
      }
      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath3.c_str(), content3.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      } else {
        Serial.println(fbdo.errorReason());
      }
    }

    //relay
    else if (type == 2) {
      String documentPath4 = "scheduler/air_pump";
      String mask1 = "switch";
      String documentPath5 = "scheduler/cooling_fan";
      String mask2 = "switch";
      String documentPath6 = "scheduler/grow_light";
      String mask3 = "switch";
      String documentPath7 = "scheduler/water_pump";
      String mask4 = "switch";

      if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath4.c_str(), mask1.c_str())) {
        String paaay = String(fbdo.payload().c_str());
        StaticJsonDocument<256> resultdata;

        deserializeJson(resultdata, paaay);
        JsonObject obj = resultdata.as<JsonObject>();

        String result = obj["fields"]["switch"]["booleanValue"];
        Serial.println(result);

        if (result == "false") {
          sendr1 = 0;
        } else {
          sendr1 = 1;
        }


      } else {
        Serial.println(fbdo.errorReason());
      }

      if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath5.c_str(), mask2.c_str())) {
        String paaay = String(fbdo.payload().c_str());
        StaticJsonDocument<256> resultdata;

        deserializeJson(resultdata, paaay);
        JsonObject obj = resultdata.as<JsonObject>();

        String result = obj["fields"]["switch"]["booleanValue"];
        Serial.println(result);

        if (result == "false") {
          sendr2 = 0;
        } else {
          sendr2 = 1;
        }


      } else {
        Serial.println(fbdo.errorReason());
      }
      if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath6.c_str(), mask3.c_str())) {
        String paaay = String(fbdo.payload().c_str());
        StaticJsonDocument<256> resultdata;

        deserializeJson(resultdata, paaay);
        JsonObject obj = resultdata.as<JsonObject>();

        String result = obj["fields"]["switch"]["booleanValue"];
        Serial.println(result);

        if (result == "false") {
          sendr3 = 0;
        } else {
          sendr3 = 1;
        }


      } else {
        Serial.println(fbdo.errorReason());
      }
      if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath7.c_str(), mask4.c_str())) {
        String paaay = String(fbdo.payload().c_str());
        StaticJsonDocument<256> resultdata;

        deserializeJson(resultdata, paaay);
        JsonObject obj = resultdata.as<JsonObject>();

        String result = obj["fields"]["switch"]["booleanValue"];
        Serial.println(result);

        if (result == "false") {
          sendr4 = 0;
        } else {
          sendr4 = 1;
        }


      } else {
        Serial.println(fbdo.errorReason());
      }

      doc_send["type"] = 2;
      doc_send["relay1"] = sendr1;
      doc_send["relay2"] = sendr2;
      doc_send["relay3"] = sendr3;
      doc_send["relay4"] = sendr4;
      serializeJson(doc_send, send_jsondata);

      esp_now_send(Universal, (uint8_t *)send_jsondata.c_str(), send_jsondata.length());


      Serial.println(send_jsondata);

        send_jsondata = "";
    }
    else{
      Serial.println("error");
    }

  delay(2000);
    
  }
}
