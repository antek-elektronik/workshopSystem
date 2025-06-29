/*

Version 1.2

changelog:

- version 1.0
  basic functionality of the graphics card

-version 1.1
  redesigned how Serial port is begin read. Added special commands "\\c" for clearing, "\\n" for newline, "\\l" for drawing line (experimental, dosent work)

-version 1.2
  sending data is now much faster (set TV.delay to 0), added failsafe (shorting A0 to ground stops the main loop)
*/


#include <TVout.h>
#include <fontALL.h>

TVout TV;

int function = 0; // what function arduino currently does
int functionCount = 0;

int x0 = 0;
int x1 = 0;
int y0 = 0;
int y1 = 0;
//int len = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(TV.begin(NTSC), DEC);
  TV.select_font(font4x6);
  
  pinMode(A0, INPUT_PULLUP);
  
  TV.println("screen test");
  TV.delay(1000);
  TV.fill(WHITE);
  TV.delay(1000);
  TV.clear_screen();

 
  
}

void loop() {
  if(Serial.available() > 0){
    if(function == 0){
      char data = (char)Serial.read();
      if(data == '\\'){ function = 1; }
      else { TV.print(data); }
    } else if(function == 1){
      //special command
      char command = (char)Serial.read();
      if(command == 'c'){
        TV.clear_screen();
        TV.set_cursor(0, 0);
      } else if(command == 'l') {
        function = 2;
        functionCount = 0;
        x0 = 0;
        x1 = 0;
        y0 = 0;
        y1 = 0;
      }
      function = 0;
    } else if(function == 2){
      switch(functionCount){
        case 0:
          x0 = (int)Serial.read();
          break;
        case 1:
          y0 = (int)Serial.read();
          break;
        case 2:
          x1 = (int)Serial.read();
          break;
        case 3:
          y1 = (int)Serial.read();
          break;
        case 4:
          TV.draw_line(x0, y0, x1, y1, WHITE);
          function = 0;
          break;
      }
      functionCount ++;  
    }
  }
  TV.delay(0);

  if(digitalRead(A0) == LOW){ // failsafe
    TV.clear_screen();
    TV.print("failsafe stop");
    while(true){
      
      delay(10);
    }
  }
  
/*
  while(Serial.available() < 1){ TV.delay(10); }

  String data = Serial.readString();
  Serial.println();
  Serial.println(data.length());
  
  char characters[data.length()];
  for(int i = 0; i < data.length(); i++){
    characters[i] = data[i];
    Serial.print(characters[i]);
  }

  TV.println(characters);
  TV.delay(1);

  TV.delay(2500);
*/

}


