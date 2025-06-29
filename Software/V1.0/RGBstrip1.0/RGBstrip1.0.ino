#define R 9
#define G 10
#define B 11

#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 6); // RX, TX

void sendSlow(String data){
  Serial.println("\nSending...");
  String dataToSend = data;
  for(int i = 0; i < dataToSend.length(); i++){
    if((dataToSend[i] == '\\')&&(dataToSend[i+1] == 'n')){
      mySerial.println();
      i++;
    } else{
      mySerial.print(dataToSend[i]);
      Serial.print(dataToSend[i]);
    }
    delay(4);
  }
  Serial.println();
}

void high(){
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);
}

void low(){
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
}

void idle(){
  for(int i = 0; i < 256; i++){
    analogWrite(R, 255-i);
    analogWrite(G, 255-i);
    analogWrite(B, 255-i);
    delay(10);
  }
  low();
}
void exitIdle(){
    for(int i = 0; i < 256; i++){
    analogWrite(R, i);
    analogWrite(G, i);
    analogWrite(B, i);
    delay(10);
  }
  high();
}

void policeSirens(){
  high();
  for(int i = 0; i < 255; i++){
    analogWrite(R, 255-i);
    if(i%2==0){delay(1);};
  }
    for(int i = 0; i < 255; i++){
    analogWrite(R, i);
    if(i%2==0){delay(2);};
  }
  high();
  
  for(int i = 0; i < 255; i++){
    analogWrite(B, 255-i);
    if(i%2==0){delay(1);};
  }
    for(int i = 0; i < 255; i++){
    analogWrite(B, i);
    if(i%2==0){delay(2);};
  }

  high();
}

void analogTest(){
  exitIdle();
  high();
  for(int i = 0; i < 255; i++){
    analogWrite(R, 255-i);
    delay(5);
  }
  for(int i = 0; i < 255; i++){
    analogWrite(R, i);
    delay(5);
  }
  
  for(int i = 0; i < 255; i++){
    analogWrite(G, 255-i);
    delay(5);
  }
  for(int i = 0; i < 255; i++){
    analogWrite(G, i);
    delay(5);
  }
  
  for(int i = 0; i < 255; i++){
    analogWrite(B, 255-i);
    delay(5);
  }
  for(int i = 0; i < 255; i++){
    analogWrite(B, i);
    delay(5);
  }

  idle();
}

void RGBtest(){
  exitIdle();
  high();
  digitalWrite(R, LOW);
  delay(1000);
  high();
  digitalWrite(G, LOW);
  delay(1090);
  high();
  digitalWrite(B, LOW);
  delay(1000);
  idle();
} 

void writeBinary(){
  high();
  Serial.println("Write Red Value (0/1): ");
  while(Serial.available() < 1){ delay(10); }
  bool redValue = Serial.parseInt();
  digitalWrite(R, !redValue);
  sendSlow("\\n Red: ");
  delay(10);
  sendSlow(String(redValue));

  Serial.println("Write Green Value (0/1): ");
  while(Serial.available() < 1){ delay(10); }
  bool greenValue = Serial.parseInt();
  digitalWrite(G, !greenValue);
  sendSlow("\\n Green: ");
  delay(10);
  sendSlow(String(greenValue));

  Serial.println("Write Blue Value (0/1): ");
  while(Serial.available() < 1){ delay(10); }
  bool blueValue = Serial.parseInt();
  digitalWrite(B, !blueValue);
  sendSlow("\\n Blue: ");
  delay(10);
  sendSlow(String(blueValue));

  Serial.println("Done!");
}

void setup() {
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  high();
  idle();

  Serial.begin(115200);
  mySerial.begin(115200);

  Serial.println("Welcome!");

  delay(2500);
  sendSlow("\\cHello World");
}

void loop() {
  if(Serial.available() > 0){
    String recieved = Serial.readString();

    if(recieved == "test"){
      sendSlow("\\nperforming an RGB test");
      Serial.println("performing an RGB test");
      RGBtest();
    } else if(recieved == "police"){
      exitIdle();
      Serial.println("police mode activated\nsend anything to abort");
      while(Serial.available() < 1){
        policeSirens();
      }
      idle();
    } else if(recieved == "clear"){
      
      high();
    } else if(recieved == "binary"){
      sendSlow("\\nWriting binary values");
      writeBinary();
    } else if(recieved == "write"){
      Serial.println("enter data to be sent to display CPU");
      while(Serial.available() < 1){
        delay(1);
      }
      sendSlow(Serial.readString());
      Serial.println("Sent!");
    } else if(recieved == "analogTest"){
      sendSlow("\nperforming an analog RGB test");
      analogTest();
    }
  }  
  
}
