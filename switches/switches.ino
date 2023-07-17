#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <espnow.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
const char *ssid = "AndroidAP";
const char *password = "ched12345";

#define APP_KEY "AIzaSyAXIYk1apQQ0M1a57TWNsx2_nVqRvXD3rw"
#define FIREBASE_PROJECT_ID "aqua-monitoring-system-c7b63"
#define USER_EMAIL "chedrick.follero.rowy@gmail.com"
#define USER_PASSWORD "YnKak4qw"
#define sw1 5
#define sw4 4
#define sw10 2

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
bool open1 = false;
bool close1 = true;

bool swt1;
bool swt4;
bool swt10;

bool tempsw1;
bool tempsw4;
bool tempsw10;
int count = 1;
String wifi_ssid;
void setup() {
  Serial.begin(115200);
  pinMode(sw1, INPUT);
  pinMode(sw4, INPUT);
  pinMode(sw10, INPUT);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s .", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
   
  }
  Serial.println(" ok");

  IPAddress ip = WiFi.localIP();



  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
  config.api_key = APP_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
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
  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {

    String documentPath1 = "combo_box/slot_1";
    String mask1 = "value";
    String documentPath2 = "combo_box/slot_4";
    String mask2 = "value";
    String documentPath3 = "combo_box/slot_10";
    String mask3 = "value";

    FirebaseJson content1;
    FirebaseJson content2;
    FirebaseJson content3;

    if (digitalRead(sw1) == LOW) {
      swt1 = open1;
      if (tempsw1 != swt1) {
        Serial.println("false");
        content1.set("fields/value/booleanValue", open1);
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath1.c_str(), content1.raw(), "value" /* updateMask */))
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
          Serial.println(fbdo.errorReason());

      } else {
        Serial.println("Data is the same");
      }

      delay(1000);


    } else {
      swt1 = close1;
      if (tempsw1 != swt1) {
        Serial.println("true");
        content1.set("fields/value/booleanValue", close1);
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath1.c_str(), content1.raw(), "value" /* updateMask */))
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
          Serial.println(fbdo.errorReason());
      } else {
        Serial.println("Data is the same");
      }

      delay(1000);
    }


      if (digitalRead(sw4) == LOW) {
      swt4 = open1;
      if (tempsw4 != swt4) {
        Serial.println("false");
        content2.set("fields/value/booleanValue", open1);
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath2.c_str(), content2.raw(), "value" /* updateMask */))
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
          Serial.println(fbdo.errorReason());

      } else {
        Serial.println("Data is the same");
      }

      delay(1000);


    } else {
      swt4 = close1;
      if (tempsw4 != swt4) {
        Serial.println("true");
        content2.set("fields/value/booleanValue", close1);
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath2.c_str(), content2.raw(), "value" /* updateMask */))
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
          Serial.println(fbdo.errorReason());
      } else {
        Serial.println("Data is the same");
      }

      delay(1000);
    }
         if (digitalRead(sw10) == LOW) {
      swt10 = open1;
      if (tempsw4 != swt10) {
        Serial.println("false");
        content3.set("fields/value/booleanValue", open1);
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath3.c_str(), content3.raw(), "value" /* updateMask */))
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
          Serial.println(fbdo.errorReason());

      } else {
        Serial.println("Data is the same");
      }

      delay(1000);


    } else {
      swt10 = close1;
      if (tempsw10 != swt10) {
        Serial.println("true");
        content3.set("fields/value/booleanValue", close1);
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath3.c_str(), content3.raw(), "value" /* updateMask */))
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
          Serial.println(fbdo.errorReason());
      } else {
        Serial.println("Data is the same");
      }

      delay(1000);
    }


  }


  tempsw1 = swt1;
  tempsw4 = swt4;
  tempsw10 = swt10;
}
