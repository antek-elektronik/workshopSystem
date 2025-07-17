////definitions
//TV Serial Pins
#define TV_RX 19
#define TV_TX 18

//RGB Strip Controller Pins
#define R 9
#define G 10
#define B 11
////

//joystick pins
#define JOYSTICK_X A0
#define JOYSTICK_Y A1
#define BTN A2

//delays
#define SEND_SLOW_DELAY 3
#define JOYSTICK_TASK_DELAY 50
#define MENU_TASK_WAITING_DELAY 40

//other
#define JOYSTICK_TASK_COUNTER_MAX_VALUE 2

//libraries
#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include "MenuPages.h"


SoftwareSerial TV(TV_RX, TV_TX); //create TV object

///global variables
int actualValueR = 0;
int actualValueG = 0;
int actualValueB = 0;
///


///sendSlow function variable 
int sendSlowCounter = 0;
String sendSlowText = "";
///

//handle for working with sendSlowTask
TaskHandle_t sendSlowTaskHandle = NULL;

//task for sending data to graphics card one by one. This is nessesary because of TvOut library interrupts breaking regular Serial transmissions on Graphic Card Microcontroller
void sendSlowTask(void * parameters){  
  for(;;){ //forever loop
    //Serial.println("sending shit");
    //if all of the data is send, reset the variables and suspend the task 
    if(sendSlowCounter > sendSlowText.length()){
      sendSlowCounter = 0; // reset the counter variable
      sendSlowText = ""; // reset the text variable
      if(sendSlowTaskHandle != NULL) { vTaskSuspend(sendSlowTaskHandle); } //suspend this task if handle is attached
      else{ Serial.println("FATAL ERROR: sendSlowHandle not attached"); }
    }

    //Serial.print(sendSlowText[sendSlowCounter]); //send data to a computer

    if(sendSlowText[sendSlowCounter] == '\\'){ //special character
      if(sendSlowText[sendSlowCounter + 1] == 'n'){
        TV.println(); //new line
        sendSlowCounter ++; //skip one character, because we already know what it is, and we apply special function
      } 
      else if(sendSlowText[sendSlowCounter + 1] == 't'){
        TV.print('\t'); //tab
        sendSlowCounter ++; //skip one character, because we already know what it is, and we apply special function
      }
      else if(sendSlowText[sendSlowCounter + 1] == 'c'){
        TV.print("\\c"); //this has to be this weird because of bad TV graphics card code
        sendSlowCounter ++; //skip one character, because we already know what it is, and we apply special function
      } else {
        //if special character is not recognized, just send to the graphics card
        TV.print(sendSlowText[sendSlowCounter]);
      }
    } else {
      //if it's not a special character, just send the data to the graphics card
      TV.print(sendSlowText[sendSlowCounter]);
    }

    //increment the function counter
    sendSlowCounter++;

    vTaskDelay(SEND_SLOW_DELAY / portTICK_PERIOD_MS); // delay the transmission by approximetly 4 miliseconds
  }
}

///transition function variables
int destinationValueR = 0;
int destinationValueG = 0;
int destinationValueB = 0;
int transitionDelay = 3;
///

//handle for working with transittion function
TaskHandle_t transitionTaskHandle = NULL;


void transitionTask(void * parameters){
  for(;;){
    if(actualValueR != destinationValueR){
      if(actualValueR > destinationValueR){
        actualValueR --;
      } else if(actualValueR < destinationValueR){
        actualValueR ++;
      }
    }

    if(actualValueG != destinationValueG){
      if(actualValueG > destinationValueG){
        actualValueG --;
      } else if(actualValueG < destinationValueG){
        actualValueG ++;
      }
    }

    if(actualValueB != destinationValueB){
      if(actualValueB > destinationValueB){
        actualValueB --;
      } else if(actualValueB < destinationValueB){
        actualValueB ++;
      }
    }

  /*
    Serial.print("R: ");
    Serial.print(actualValueR);
    Serial.print(" , set: ");
    Serial.print(destinationValueR);
    Serial.print(" ,    G: ");
    Serial.print(actualValueG);
    Serial.print(" , set: ");
    Serial.print(destinationValueG);
    Serial.print(" ,    B: ");
    Serial.print(actualValueB);
    Serial.print(" , set: ");
    Serial.println(destinationValueB);
  */
    analogWrite(R, actualValueR);
    analogWrite(G, actualValueG);
    analogWrite(B, actualValueB);

    vTaskDelay(transitionDelay / portTICK_PERIOD_MS);
  }
}

TaskHandle_t analogTaskHandle = NULL;

void analogTask(void * parameters){
  for(;;){
    sendSlow("\\n> czerwony\\n");
    destinationValueR = 0;
    destinationValueG = 255;
    destinationValueB = 255;
    transitionDelay = 20;
    vTaskDelay(2550/portTICK_PERIOD_MS);
    sendSlow("\\n> zielony\\n");
    destinationValueR = 255;
    destinationValueG = 0;
    destinationValueB = 255;
    transitionDelay = 20;
    vTaskDelay(2550/portTICK_PERIOD_MS);
    sendSlow("\\n> niebieski\\n");
    destinationValueR = 255;
    destinationValueG = 255;
    destinationValueB = 0;
    transitionDelay = 20;
    vTaskDelay(2550/portTICK_PERIOD_MS);
    
  }
}

//joystick function handler
TaskHandle_t joystickTaskHandle = NULL;

//joystick global variables
int joystickAction = 0; //determines what action was taken 0 - none, 1 - right, 2 - left, 3 - button press

//joystick task function
void joystickTask(void * parameters){
  for(;;){
    bool joystickLooping = true;
    int joystickCounter = 0;
    while(joystickLooping){

      joystickLooping = false; //stop looping before checking for any actions
      if(analogRead(JOYSTICK_X) > 700){
        joystickAction = 2; //left
      } else if(analogRead(JOYSTICK_X) < 300){
        joystickAction = 1; //right
      } else if(digitalRead(BTN) == LOW){ //joystick button is on pullup
        joystickAction = 3; //button pressed
      } else{
        joystickLooping = true; //if nothing else was done, just continue looping
        joystickCounter++;
        if(joystickCounter >= JOYSTICK_TASK_COUNTER_MAX_VALUE){ joystickCounter = 0; joystickAction = 0; } //if nothing happens for some time, reset the joystick action
      }

      vTaskDelay(JOYSTICK_TASK_DELAY / portTICK_PERIOD_MS); //add delay so that freeRTOS can execute other tasks in the meantime
    }
  }
}


/////// scene changing logic
void menuButtonPressed(){
  //const String* activeScene = scenes[scene].items;
  if(activeScene.items == mainMenu){
    switch(cursor){
      case 0:
        activeScene.items = scenes[findSceneNumber(*systemStatusMenu)].items;
        activeScene.length = ARRAY_SIZE(activeScene.items);
        break;
    }
  }
}



/// menu task handle
TaskHandle_t menuTaskHandle = NULL;

///menu generation task
void menuTask(void * parameters){
   for(;;){
    while(joystickAction == 0){ vTaskDelay(MENU_TASK_WAITING_DELAY / portTICK_PERIOD_MS); }

    int length = activeScene.length;

    //joystick action logic
    if(joystickAction == 1){
      cursor ++;
      if(cursor > length - 1){ cursor --; }
    } else if(joystickAction == 2){
      cursor --;
      if(cursor < 0){ cursor ++; }
    } else if(joystickAction == 3){
      menuButtonPressed();
    }

    ////SCENE GENERATION

    sendSlow("\\c"); //clear screen
    vTaskDelay(SEND_SLOW_DELAY * 2 / portTICK_PERIOD_MS);

     if(findSceneNumber(activeScene) < (sizeof(scenes)/sizeof(scenes[0]))){
      const Scene& s = activeScene;
      length = s.length;
      if(s.items != nullptr){
        for(int i = 0; i < length; i++){
          sendSlow(s.items[i]);
          vTaskDelay((SEND_SLOW_DELAY * (s.items[i].length()) + 5) / portTICK_PERIOD_MS); //delay for calculated amount of time  (that would take to send the data) + 5ms for safety
          if(i == cursor){ ///printing cursor
            int number_of_spaces = 25 - s.items[i].length();
            if(number_of_spaces < 0){ number_of_spaces = 0; }
            String spaces = "";
            for(int j = 0; j < number_of_spaces; j++){ spaces += " "; }
            sendSlow(spaces + "<");
            vTaskDelay(SEND_SLOW_DELAY * number_of_spaces / portTICK_PERIOD_MS);
          }

          sendSlow("\\n");
          vTaskDelay(SEND_SLOW_DELAY * 2 / portTICK_PERIOD_MS);
        }
      }
    }

    ///////debuging
    Serial.println("Scene generation complete, evaluation: ");
    Serial.print("length: ");
    Serial.print(length);
    Serial.print("   cursor: ");
    Serial.print(cursor);
    Serial.print("   joystickAction: ");
    Serial.print(joystickAction);
    Serial.print("    ActiveScene: ");
    Serial.print(findSceneNumber(activeScene));
    Serial.println();


    while(joystickAction != 0){ vTaskDelay(MENU_TASK_WAITING_DELAY / portTICK_PERIOD_MS); }
   }
}


///// regular functions

void sendSlow(String dataToSend){
  sendSlowCounter = 0; //reset send slow function counter
  sendSlowText = dataToSend; //set send slow text variable to desired data to send
  vTaskResume(sendSlowTaskHandle); // resume sendSlowTask
}

void setup() {
  
  //pin definitions
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);

  pinMode(BTN, INPUT_PULLUP);

  //begin Serial connections
  Serial.begin(921600);
  TV.begin(115200);

  Serial.println("Welcome!");

  //Create Tasks
  xTaskCreate(
    sendSlowTask,
    "sendSlow",
    128,
    NULL,
    1,
    &sendSlowTaskHandle
  );
  vTaskSuspend(sendSlowTaskHandle);

  xTaskCreate(
    transitionTask,
    "transition",
    128,
    NULL,
    1,
    &transitionTaskHandle
  );

  /*
  xTaskCreate(
    analogTask,
    "analogTest",
    128,
    NULL,
    1,
    NULL
  );
*/
  
  xTaskCreate(
    joystickTask,
    "joystick",
    128,
    NULL,
    1,
    &joystickTaskHandle
  );

  xTaskCreate(
    menuTask,
    "menu",
    128,
    NULL,
    1,
    &menuTaskHandle
  );
  

  sendSlow("\\c RGBStrip2.0\\n\\nmade by Antoni Gzara\\n\\n\\n\\nloading...");
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
