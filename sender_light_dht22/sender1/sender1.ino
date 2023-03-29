#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <espnow.h>
#include <FirebaseESP8266.h> 
#define FIREBASE_HOST "aqua-monitoring-system-default-rtdb.firebaseio.com"
//Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "RNZLaAyQqslv3gUpCBtYJcayk6FzObHfSipY2D6k" 
//Your Firebase Database Secret goes here
#define WIFI_SSID "ched_wifi"                                              
//WiFi SSID to which you want NodeMCU to connect
#define WIFI_PASSWORD "YnKak4qw"                                      
//Password of your wifi network 
// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;
// Declare global variable to store value

uint8_t broadcastAddress[] = {0x48, 0x55, 0x19, 0xC8, 0x63, 0x61};

int pinDHT22 = 5;
SimpleDHT22 dht22(pinDHT22);
int lightPen=A0;

typedef struct struct_message1 {
  int lightVal;
  byte temperature = 0;
  byte humidity = 0;

} struct_message1;
struct_message1 SENDER1;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
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

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());//print local IP address

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);// connect to firebase
  Firebase.reconnectWiFi(true);
  delay(1000);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

}

void loop() {

  

  Serial.println("=================================");
  Serial.println("Data");

  
  int err = SimpleDHTErrSuccess;
  SENDER1.lightVal = analogRead(lightPen); 
  if ((err = dht22.read(&SENDER1.temperature, &SENDER1.humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(2000);
    return;
  }

  Serial.print("Sample OK: ");
  Serial.print((int)SENDER1.temperature); Serial.print(" *C, ");
  Serial.print((int)SENDER1.humidity); Serial.println(" %");
  Serial.print((int)SENDER1.lightVal); Serial.println(" ");


  esp_now_send(broadcastAddress, (uint8_t *) &SENDER1, sizeof(SENDER1));

  lastTime = millis();
  delay(2500);
  String temp = String(SENDER1.temperature);
  String hum = String(SENDER1.humidity);
  String lss = String(SENDER1.lightVal);
  
     FirebaseData fbdo;


if (Firebase.setString(fbdo, "/sensors_monitoring/sensors1/value_hum", hum)){
  Serial.println("set array for humidity monitoring");
  }else{
    Serial.println(fbdo.errorReason());
  }
if (Firebase.setString(fbdo, "/sensors_monitoring/sensors1/value_tem", temp)){
  Serial.println("set array for temperature monitoring");
  }else{
    Serial.println(fbdo.errorReason());
  }

 
  //lss

if (Firebase.setString(fbdo, "/sensors_monitoring/sensors1/value_lss", lss)){
  Serial.println("set array for lightVal monitoring");
  }else{
    Serial.println(fbdo.errorReason());
  }

}




