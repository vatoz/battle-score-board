#include "PCF8574.h"
#include <Wire.h>

// adjust addresses if needed
PCF8574 PCF_left(0x23); 
PCF8574 PCF_right(0x22);

PCF8574 PCF_left2(0x20);
PCF8574 PCF_right2(0x24);

unsigned char mask[4];
unsigned char driver_left;
unsigned char driver_right;
unsigned char driver_left2;
unsigned char driver_right2;

unsigned char val_left;
unsigned char val_right;
unsigned char val_foul_left;
unsigned char val_foul_right;

int pinUpL=9;
int pinUpR=5;
int pinDownL=10;
int pinDownR=6;
int pinFoulL=8;
int pinFoulR=7;
int led=13;


void setup() {
  
  val_left=0;
  val_right=0;
  val_foul_left=0;
  val_foul_right=0;
  
  Wire.begin();
  setDisplay();

  pinMode(pinUpL, INPUT_PULLUP);  
  pinMode(pinUpR, INPUT_PULLUP);  
  pinMode(pinDownL, INPUT_PULLUP);  
  pinMode(pinDownR, INPUT_PULLUP);  
  pinMode(pinFoulL, INPUT_PULLUP);  
  pinMode(pinFoulR, INPUT_PULLUP);    

  pinMode(LED_BUILTIN, OUTPUT);    
}

void handleInputs(){
  int temp=0;
  char change=0;
  if (!digitalRead(pinUpL)){
    if (val_left<19 ) val_left++;     
    change=1;    
  }
  if (!digitalRead(pinUpR)){
    if (val_right<19  ) val_right++;     
    change=1;    
  }

  if (!digitalRead(pinDownL)){
    if (val_left>0  ) val_left--;     
    change=1;    
  }
  if (!digitalRead(pinDownR)){
    if (val_right>0  ) val_right--;     
    change=1;    
  }

 if (!digitalRead(pinFoulR)){
    if(val_foul_right<2) {
        val_foul_right++;
        change=1;
      }else{
      for(temp =0;temp<5;temp++){
        val_foul_right=3;
        setDisplay();
        delay(200);
        val_foul_right=0;
        setDisplay();
        delay(200);
        }  
       if (val_left<19  ) val_left++;    
       change=1; 
      }
 }

if (!digitalRead(pinFoulL)){
    if(val_foul_left<2) {
        val_foul_left++;
        change=1;
      }else{
      for(temp =0;temp<5;temp++){
        val_foul_left=3;
        setDisplay();
        delay(200);
        val_foul_left=0;
        setDisplay();
        delay(200);
        }  
       if (val_right<19  ) val_right++;    
       change=1; 
      }
 }

  
  if (change){    
    digitalWrite(LED_BUILTIN,HIGH);
    setDisplay();
    delay(800) ;   
    
    digitalWrite(LED_BUILTIN,LOW);
    }
}



void setDisplay(){
  const unsigned char numbers[]={
    63//0
    ,24
    ,118
    ,124
    ,1+8+16+64//4
    ,1+64+4+32+8
    ,1+2+4+8+32+64
    ,8+16+32
    ,127
    ,125};
    driver_left2=0;
    driver_right2=0;
    if(val_left>9){
      driver_left=numbers[val_left-10];
      driver_left2=3;
    }else{
      driver_left=numbers[val_left];
    }
    if(val_right>9){
      driver_right=numbers[val_right-10];
      driver_right2=3;
    }else{
      driver_right=numbers[val_right];
    }  

  switch(val_foul_left){
  case 0:
    break;
  case 1:
    driver_left2+=16;
    break;
  case 2:
    driver_left2+=24;
    break;
  case 3:
    driver_left2+=28;
    break;  
  }

  switch(val_foul_right){
  case 0:
    break;
  case 1:
    driver_right2+=16;
    break;
  case 2:
    driver_right2+=24;
    break;
  case 3:
    driver_right2+=28;
    break;  
  }
    driver_left2+=32;
   
    sendtodisplay();
}

void sendtodisplay(){
   PCF_left.write8(driver_left);
   PCF_right.write8(driver_right);
   PCF_left2.write8(driver_left2);
   PCF_right2.write8(driver_right2);

}

void test(){
  for(int i=0;i<8;i++){
      PCF_right2.write8(1<<i);
      delay(300);
    }
      delay(1000);
  }
  
void loop() {
   handleInputs();  
   sendtodisplay();
   //test();
}

