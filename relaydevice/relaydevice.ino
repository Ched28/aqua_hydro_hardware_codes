#include <ESP8266WiFi.h>

#include <espnow.h>
#include <ArduinoJson.h>
//


// recieve incoming and sending data struct: main_esp_data, relay_esp_data, user_esp_data - broadcast
// recieve incoming only: sensor1_esp_data, sensor2_esp_data - no need for mac address
// TYPES: 1 - main_esp_data, 2 - relay_esp_data, 3 - user_esp_data, 4 - sensor1_esp_data, 5 - sensor2_esp_data

//----------------------------------------------------------------------------------------------------------------//

//sensor1
// send only



uint8_t Universal[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

String recv_jsondata;
String send_jsondata;

StaticJsonDocument<256> doc_send;
StaticJsonDocument<256> doc_recv;

const int relay1 = 5;  // GPIO5 D1
const int relay2 = 4;  // GPIO4 D2
const int relay3 = 0;  // GPIO0 D3
const int relay4 = 2;  // GPIO2 D4



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

String R1;
String R2;
String R3;
String R4;


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");

  } else {
    Serial.println("Delivery fail");
  }
}
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
    if (type2 == 2) {
    R1 = String(doc_recv["relay1"]);
    R2 = String(doc_recv["relay2"]);
    R3 = String(doc_recv["relay3"]);
    R4 = String(doc_recv["relay4"]);
    myIncomingrelaydata.Relay1 = R1.toInt();
    myIncomingrelaydata.Relay2 = R2.toInt();
    myIncomingrelaydata.Relay3 = R3.toInt();
    myIncomingrelaydata.Relay4 = R4.toInt();
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

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  // Set outputs to HIGH. relay active LOW
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  myrelaydata.Relay1 = 0;
  myrelaydata.Relay2 = 0;
  myrelaydata.Relay3 = 0;
  myrelaydata.Relay4 = 0;


  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);


  esp_now_add_peer(Universal, ESP_NOW_ROLE_COMBO, 6, NULL, 0);

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {


  if (myIncomingrelaydata.Relay1 != myrelaydata.Relay1) {
    myrelaydata.Relay1 = myIncomingrelaydata.Relay1;
    if (myrelaydata.Relay1 == 0) {
      digitalWrite(relay1, HIGH);
    }
    else{
      digitalWrite(relay1, LOW);
    }
  }else{
     myrelaydata.Relay1 =  myrelaydata.Relay1;
  }
  if (myIncomingrelaydata.Relay2 != myrelaydata.Relay2) {
    myrelaydata.Relay2 = myIncomingrelaydata.Relay2;
    if (myrelaydata.Relay2 == 0) {
      digitalWrite(relay2, HIGH);
    }
    else{
      digitalWrite(relay2, LOW);
    }
  }else{
    myrelaydata.Relay2 = myrelaydata.Relay2;
  }
  if (myIncomingrelaydata.Relay3 != myrelaydata.Relay3) {
    myrelaydata.Relay3 = myIncomingrelaydata.Relay3;
    if (myrelaydata.Relay3 == 0) {
      digitalWrite(relay3, HIGH);
    }
    else {
      digitalWrite(relay3, LOW);
    }
  }else{
    myrelaydata.Relay3 = myrelaydata.Relay3;
  }
  if (myIncomingrelaydata.Relay4 != myrelaydata.Relay4) {
    myrelaydata.Relay4 = myIncomingrelaydata.Relay4;
    if (myrelaydata.Relay4 == 0) {
      digitalWrite(relay4, HIGH);
    }
    else{
      digitalWrite(relay4, LOW);
    }
  }else{
    myrelaydata.Relay4 = myrelaydata.Relay4;
  }





  doc_send["type"] = 2;
  doc_send["relay1"] = myrelaydata.Relay1;
  doc_send["relay2"] = myrelaydata.Relay2;
  doc_send["relay3"] = myrelaydata.Relay3;
  doc_send["relay4"] = myrelaydata.Relay4;
  serializeJson(doc_send, send_jsondata);



  esp_now_send(Universal, (uint8_t *)send_jsondata.c_str(), send_jsondata.length());
  Serial.println(send_jsondata);

  send_jsondata = "";
  delay(1000);
}
