//turn all pins high
void high(){ 
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);
}

//turn all pins low
void low(){
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
}

void light(){ low(); } //turn all leds on
void dark(){ high(); } //turn all leds off

