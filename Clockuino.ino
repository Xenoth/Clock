
#include <SPI.h>
#include <Gamebuino.h>
#include "Clock.h"

const char* const mainMenu[] PROGMEM = {"Resume","Setup Clock","Setup Alarm"};

uint8_t second, minute, hour;
bool pm, mode12h;

Gamebuino gb;
Clock cl;
int8_t sleep=0;
boolean locked=false;

void printClock()
{
  if(mode12h)
  {
    gb.display.cursorX = 5;
    gb.display.cursorY = 24;
    gb.display.fontSize = 5;
    if(pm)
      gb.display.print("pm");
     else
      gb.display.print("am");
  }
  gb.display.cursorX = 10;
  gb.display.cursorY = 19;
  gb.display.fontSize = 10;
  gb.display.print(hour);
  if(second%2)
  {
    gb.display.cursorX = 25;
    gb.display.print(":");
  }
  gb.display.cursorX = 30;
  gb.display.print(minute);
}

void notDetected()
{
  gb.display.clear();
  gb.display.cursorX = 10;
  gb.display.cursorY = 19;
  gb.display.print("Module not detected !");
  gb.sound.playTick();
}


void setup() {
  gb.begin();
  Wire.begin();
  gb.sound.playOK();
  gb.titleScreen(F("Clockuino"));
}

void loop()
{
 if(gb.update())
 {  

   /*
   if(gb.buttons.pressed(BTN_A) && locked)
   {
     locked=false;
     gb.backlight.automatic=true;
     gb.setFrameRate(20);
     sleep=0;
   }
   else if(!locked)
   {
     if(gb.buttons.pressed(BTN_A))
     {
      sleep=0;
     }
  
     else if(gb.buttons.pressed(BTN_C))
     {
      sleep=0;
      gb.titleScreen(F("Clockuino"));
     }
  
     else if(gb.buttons.pressed(BTN_B))
     {
      sleep=0;
      int8_t mainMenuRes=gb.menu(mainMenu, 3);
      switch(mainMenuRes)
      {
        case 1:
        //CHANGER Heure
          break;

        case 2:
        //CHANGER Alarme
          break;
      }
      
      }
     }
     
     else
     {
       sleep++;
       if(sleep==100)
       {
          locked=true;
          gb.backlight.automatic=false;
          gb.backlight.set(0);
          gb.setFrameRate(2);
       }
     }*/

   cl.getCurrentTime(&mode12h, &pm, &hour, &minute, &second);
   if(cl.isDetected())
      printClock();
   else
      notDetected();
   }
   
 }
