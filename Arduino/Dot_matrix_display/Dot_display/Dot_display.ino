//Mario's Ideas
//MAX7219 - using Led Control library to display all leds one by one
#include <LedControl.h>

int DIN = 11;
int CS = 10;
int CLK = 13;

LedControl lc=LedControl(DIN, CLK, CS,0);



void setup() {

  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
}

void loop() {
lc.setLed(0,0,0,true);
display_1();
  
//  for(int j=0;j<8;j++){
//     for(int i=0;i<8;i++){
//       lc.setLed(0,j,i,true);
//       delay(100);
//       lc.setLed(0,j,i,false);
//     }
//  }
}


void display_1(){
  for (int i = 0; i < 8; ++i){
  lc.setLed(0,i,3,true);
  lc.setLed(0,i,4,true);
}
delay (1000);
 for (int i = 0; i < 8; ++i){
  lc.setLed(0,i,3,false);
  lc.setLed(0,i,4,false);
}
delay (500);
}
