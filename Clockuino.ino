
#include <SPI.h>
#include <Gamebuino.h>
#include "DS3231.h"

const char day1[] PROGMEM = "Monday";
const char day2[] PROGMEM = "Tuesday";
const char day3[] PROGMEM = "Wednesday";
const char day4[] PROGMEM = "Thursday";
const char day5[] PROGMEM = "Friday";
const char day6[] PROGMEM = "Saturday";
const char day7[] PROGMEM = "Sunday";

const char* const week[] PROGMEM = {day1, day2, day3, day4, day5, day6, day7};


const char month1[] PROGMEM = "January";
const char month2[] PROGMEM = "February";
const char month3[] PROGMEM = "March";
const char month4[] PROGMEM = "April";
const char month5[] PROGMEM = "May";
const char month6[] PROGMEM = "June";
const char month7[] PROGMEM = "July";
const char month8[] PROGMEM = "August";
const char month9[] PROGMEM = "September";
const char month10[] PROGMEM = "October";
const char month11[] PROGMEM = "November";
const char month12[] PROGMEM = "December";

const char* const monthes[] PROGMEM = {month1, month2, month3, month4, month5, month6, month7, month8, month9, month10, month11, month12};


const char menu0_choice0[] PROGMEM = "Current Date";
const char menu0_choice1[] PROGMEM = "Set up Date";
const char menu0_choice2[] PROGMEM = "Time Settings";
const char menu0_choice3[] PROGMEM = "Alarm Settings";

const char* const menu0[] PROGMEM = {menu0_choice0, menu0_choice1, menu0_choice2, menu0_choice3};

const char menu1_choice0[] PROGMEM = "Switch mode12h/24h[wip]";
const char menu1_choice1[] PROGMEM = "Set Time";

const char* const menu1[] PROGMEM = {menu1_choice0, menu1_choice1};

const char menu2_choice0[] PROGMEM = "Alarm on/off[wip]";
const char menu2_choice1[] PROGMEM = "Set Alarm[wip]";

const char* const menu2[] PROGMEM = {menu2_choice0, menu2_choice1};

uint8_t second, minute, hour;
bool pm, mode12h;

uint8_t day_of_week=1, day_of_month=1, month=1, year=1;

Gamebuino gb;
DS3231 cl;

uint8_t sleep=0;
boolean locked;



void printDate(uint8_t day_of_week, uint8_t day_of_month, uint8_t month, uint8_t year)
{
  while(1)
  {
    if(gb.update())
    {
      gb.display.cursorX = 5;
      gb.display.cursorY = 10;
      gb.display.println((String)((const __FlashStringHelper*)pgm_read_word(week+day_of_week-1))+" "+(String)(day_of_month)+" "+(String)((const __FlashStringHelper*)pgm_read_word(monthes+month-1)));
      gb.display.cursorY = 20;
      gb.display.cursorX = 65;
      gb.display.print(year+2000);
      if(gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        break; 
      }
    }
  }
}


void printClock(uint8_t second, uint8_t minute, uint8_t hour, bool mode12h, bool pm)
{
  if(mode12h)
  {
    gb.display.cursorX = 5;
    gb.display.cursorY = 24;
    if(pm)
      gb.display.print("PM");
     else
      gb.display.print("AM");
  }
  gb.display.cursorX = 15;
  gb.display.cursorY = 19;
  gb.display.fontSize = 2;
  gb.display.print(hour);
  if(second%2)
  {
    gb.display.cursorX = 30;
    gb.display.print(":");
  }
  gb.display.cursorX = 40;
  gb.display.print(minute);
  gb.display.fontSize = 1;
}

void notDetected()
{
  gb.display.cursorX = 1;
  gb.display.cursorY = 19;
  gb.display.print("Module not detected !");
  if (locked && (!(gb.frameCount%4))) 
    gb.sound.playNote(20, 1, 0);
  else if (!(gb.frameCount%20))
    gb.sound.playNote(20, 5, 0);  
}

uint8_t set_Time(bool alarm)
{
  gb.display.persistence = false;
  cl.getCurrentTime(&mode12h, &pm, &hour, &minute, &second);
  uint8_t select=0;
  while(1)
  {
    if (gb.update())
    {
      printClock(1, minute, hour, mode12h, pm);
      gb.display.fillTriangle(19+(25*select), 15, 25+(25*select), 15, 22+(25*select), 12);
      gb.display.fillTriangle(19+(25*select), 31, 25+(25*select), 31, 22+(25*select), 34);
      if(gb.buttons.repeat(BTN_UP, 5))
      {
        gb.sound.playTick();
        if(select==0)
        {
          hour++;
          if((!mode12h) && hour==24)
            hour=0;
          else if (mode12h && hour>12)
          {
            hour=1;
            if(pm)
              pm=false;
            else
              pm=true;
          }
        }
        else
        {
          minute++;
          if(minute>59)
            minute=0;
        }
      }
      else if(gb.buttons.repeat(BTN_DOWN, 5))
      {
        gb.sound.playTick();
        if(select==0)
        {
          if((!mode12h) && hour==0)
            hour=23;
          else if (mode12h && hour==0)
          {
            hour=12;
            if(pm)
              pm=false;
            else
              pm=true;
          }
          else
            hour=hour-1;
        }
        else
        {
          if(minute==0)
            minute=59;
          else
            minute=minute-1;
        }
      }
      else if (gb.buttons.repeat(BTN_LEFT, 1))
      {
        if (select>0)
        {
          gb.sound.playTick();
          select=0;
        }
      }
      else if (gb.buttons.repeat(BTN_RIGHT, 1))
      {
        if (select<1)
        {
          gb.sound.playTick();
          select=1;
        }
      }
      else if(gb.buttons.pressed(BTN_B))
      {
        gb.sound.playCancel();
        return -1;
      }
      else if(gb.buttons.pressed(BTN_A))
      {
        gb.sound.playOK();
        if(!alarm)
        {
          cl.setSecond(0);
          cl.setHour(hour, mode12h, pm);
          cl.setMinute(minute);
          cl.setSecond(second);
        }
        else
        {
          //setALARM
        }
        return 0;
      }
    }
  }
}

int8_t set_Date()
{
  uint8_t select=0;
  cl.getDate(&day_of_week, &day_of_month, &month, &year);
  while(1)
  {
    if(gb.update())
    {
      gb.display.cursorX = 5;
      gb.display.cursorY = 10;
      gb.display.println((String)((const __FlashStringHelper*)pgm_read_word(week+day_of_week-1))+" "+(String)(day_of_month)+" "+(String)((const __FlashStringHelper*)pgm_read_word(monthes+month-1)));
      gb.display.cursorY = 20;
      gb.display.cursorX = 65;
      gb.display.print(year+2000);
  
      if(gb.buttons.pressed(BTN_A))
      {
        gb.sound.playOK();
        cl.setDayOfMonth(day_of_month);
        cl.setMonth(month);
        cl.setYear(year);
        return 0;      
      }
      else if(gb.buttons.pressed(BTN_B))
      {
        gb.sound.playCancel();
        return 0;
      }
      else if(gb.buttons.pressed(BTN_RIGHT) && select<=3)
      {
        gb.sound.playTick();
        select++;
      }
      else if(gb.buttons.pressed(BTN_LEFT) && select>0)
      {
        gb.sound.playTick();
        select--;
      }
      else if(gb.buttons.pressed(BTN_UP))
      {
        gb.sound.playTick();
        if(select==0)
        {
          day_of_week++;
          if(day_of_week>7)
            day_of_week=1;
        }
        if(select==1)
        {
          day_of_month++;
          if(day_of_month>31)
            day_of_month=1;
        }
        if(select==2)
        {
          month++;
          if(month>12)
            month=1;
        }
        if(select==3)
        {
          year++;
          if(year>199)
            year=0;
        }
      }
      else if(gb.buttons.pressed(BTN_DOWN))
      {
        gb.sound.playTick();
        if(select==0)
        {
          day_of_week--;
          if(day_of_week==0)
            day_of_week=7;
        }
        if(select==1)
        {
          day_of_month--;
          if(day_of_month==0)
            day_of_month=31;
        }
        if(select==2)
        {
          month--;
          if(month==0)
            month=12;
        }
        if(select==3)
        {
          if(year-1<0)
            year=199;
          else
            year--;
        }
      }
    }
  }
}


void setup() {
  gb.begin();
  Wire.begin();
}

void loop()
{
 if(gb.update())
 {

   cl.getCurrentTime(&mode12h, &pm, &hour, &minute, &second);
   if(cl.isDetected())
   {
     printClock(second,minute,hour, mode12h, pm);
      if(gb.buttons.held(BTN_A, 4) && locked)
     {
       gb.battery.show = true;
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
        int8_t menu0_res=gb.menu(menu0, 4);
        switch(menu0_res)
        {
          case 0:
          {
            cl.getDate(&day_of_week, &day_of_month, &month, &year);
            printDate(day_of_week, day_of_month, month, year);
          }
          break;
          
          case 1:
          {
            set_Date();
          }
          break;
          
          case 2:
          {
            int8_t menu1_res=gb.menu(menu1, 2);
            switch(menu1_res)
            {
              case 0:
              {
                //SWITCH MODE12H
              }
              break;
               
              case 1:
              {
                set_Time(false);
              }
              break;
            }
          }
          break;
  
          case 3:
          {
            int8_t menu2_res=gb.menu(menu2, 2);
            switch(menu2_res)
            {
              case 0:
              {
                //Switch ALARM
              }
              break;
               
              case 1:
              {
                set_Time(true);
              }
              break;
            }
          }
          break;
        }
       }     
       else
       {
         sleep++;
         if(sleep==200)
         {
            gb.popup(F("HOLD A TO UNLOCK"), 2);
            locked=true;
            gb.battery.show = false;
            gb.backlight.automatic=false;
            gb.backlight.set(0);
            gb.setFrameRate(2);
         }
       }
     }
   }
   else
    notDetected();
 }
 }

