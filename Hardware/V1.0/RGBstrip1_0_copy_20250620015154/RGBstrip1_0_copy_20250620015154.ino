#define R 9
#define G 10
#define B 11

void high(){
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);
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

void RGBtest(){
  high();
  digitalWrite(R, LOW);
  delay(1000);
  high();
  digitalWrite(G, LOW);
  delay(1090);
  high();
  digitalWrite(B, LOW);
  delay(1000);
} 

void writeBinary(){
  high();
  Serial.println("Write Red Value (0/1): ");
  while(Serial.available() < 1){ delay(10); }
  bool redValue = Serial.parseInt();
  digitalWrite(R, !redValue);

  Serial.println("Write Green Value (0/1): ");
  while(Serial.available() < 1){ delay(10); }
  bool greenValue = Serial.parseInt();
  digitalWrite(G, !greenValue);

  Serial.println("Write Blue Value (0/1): ");
  while(Serial.available() < 1){ delay(10); }
  bool blueValue = Serial.parseInt();
  digitalWrite(B, !blueValue);

  Serial.println("Done!");
}

void setup() {
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    String recieved = Serial.readString();

    if(recieved == "test"){
      Serial.println("performing an RGB test");
      RGBtest();
    } else if(recieved == "police"){
      Serial.println("police mode activated\nsend anything to abort");
      while(Serial.available() < 1){
        policeSirens();
      }
    } else if(recieved == "clear"){
      high();
    } else if(recieved == "binary"){
      writeBinary();
    } else if(recieved == "transition"){
      
    }
  }  
  
}
