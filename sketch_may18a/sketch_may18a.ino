#define sw1 5
#define sw4 4
#define sw10 2



void setup() {
  Serial.begin(115200);
  pinMode(sw1, INPUT);
  pinMode(sw4, INPUT);
  pinMode(sw10, INPUT);
}

void loop() {
 if(digitalRead(sw1) == LOW){
      Serial.println("SWT1 = true");
 
 }else{
    Serial.println("SWT1 = false");
 }
 if(digitalRead(sw4) == LOW){
      Serial.println("SWT4 = true");
 
 }else{
    Serial.println("SWT4 = false");
 }

}
