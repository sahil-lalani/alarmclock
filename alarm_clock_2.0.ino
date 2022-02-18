#include <TimeLib.h>
#include <LiquidCrystal.h>
#include "pitches.h"

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int buttonCounter = 0;

int setMonth = 0; //for setting the initial DATE
int setDay = 0;
int setYear = 0;
//after i get all the information for date & time, i will set the time. later on, when i get a ds3231, i can store in there so
//i don't have to set the time up after the arduino has been unplugged

int realHour = 20; // used for changing between AM and PM

int setHour = 0; //for TIME
int setMin = 0;
int setSec = 0;

int militaryHour = 0;

int alarmHour = 7;
int alarmMilitaryHour = 7;
int alarmMinute = 0;

void setup() {
  Serial.begin(9600);
  lcd.display();
  lcd.begin(16, 2);
  initialDate();
  pinMode(A3, INPUT); // +
  pinMode(A4, INPUT); // -
  pinMode(A5, INPUT); //menu control
  pinMode(6, INPUT); //backlight control
  pinMode(13, OUTPUT); //backlight
  lcd.setCursor(1, 2);
  //setSyncProvider();
}

void loop() {
  Serial.println(digitalRead(6));
  if (digitalRead(6) == 1) { //backlight control
    if (analogRead(A0) < 900) { //if backlight is OFF
      analogWrite(A0, 255); //turn it ON
    }

    else { //if backlight is ON
      analogWrite(A0, 0); //turn it OFF
    }
    delay(250);
  }

  if (analogRead(A5) > 400) { //must hold button for 0.5 seconds to "toggle" the menu
    buttonCounter++;
    delay(250); //CONTROLS HOW LONG THE BUTTON MUST BE HELD TO TOGGLE THE MENU
    Serial.println(buttonCounter);
  }

  if (buttonCounter == 0) { //first menu: set up month
    lcd.blink();
    adjMonth();
  }

  //create button counter values all the way up to 7 (the menu where date and time will be fully set). after this, i can use modulus to switch between regular date & time and alarm.
  else if (buttonCounter == 1) { //second menu: set up day
    lcd.blink();
    adjDay();
  }

  else if (buttonCounter == 2) { //third menu: set up year
    lcd.blink();
    adjYear();
  }

  else if (buttonCounter == 3) { //fourth menu: show time
    lcd.noBlink();
    lcd.home();
    initialTime();
  }

  else if (buttonCounter == 4) { //fifth: set up hour
    lcd.blink();
    adjHour(); //figured out AM + PM setting on 2/14/22
  }

  else if (buttonCounter == 5) { //sixth: setting up minute
    lcd.blink();
    adjMin(); //SAME CODE FOR BOTH MIN AND SEC
  }

  else if (buttonCounter == 6) { //7th: setting up second
    lcd.blink();
    adjSec(); //SAME CODE FOR BOTH MIN AND SEC
  }

  else if (buttonCounter == 7) { //clear lcd and sync time
    timeSync();
  }

  else { //shows data & time menu and alarm menu
    //for setting off alarm
    //ADD CODE TO SNOOZE ALARM. IT SHOULD STOP PLAYING THE SOUND + GO BACK TO REGULAR MENU
    if (alarmMilitaryHour == hour() && alarmMinute == minute() && second() < 30) { //play james bond theme TWICE
      JamesBond();
    }


    //all the different menus
    if (buttonCounter % 4 == 0) { //show date & time
      lcd.noBlink();
      lcd.noCursor();
      displayDateAndTime();
    }
    else if (buttonCounter % 4 == 1) { //show alarm
      lcd.noBlink();
      lcd.noCursor();
      displayAlarm();
    }

    else if (buttonCounter % 4 == 2) { //adj alarm hour
      lcd.blink();
      adjAlarmHour();
    }

    else if (buttonCounter % 4 == 3) { //adj alarm minute
      lcd.blink();
      adjAlarmMinute();
    }
  }
}


void initialDate() { //this is for when the alarm is first plugged in. the screen will ALWAYS go to this. ofc, we will add an RTC later on
  setTime(1642968485); //sometime on Jan 23rd, 2022 (when this alarm clock was being developed).
  setMonth = month();
  setDay = day();
  setYear = year();
  lcd.print("Date Setup: "); //will set up date first
  lcd.setCursor(0, 2);
  if (month() < 10) {
    lcd.print("0");
  }
  lcd.print(setMonth);
  lcd.print("/");
  lcd.print(setDay);
  lcd.print("/");
  lcd.print(setYear);
  lcd.setCursor(0, 2);
}

void initialTime() { //sometime on Jan 23rd, 2022 (when this alarm clock was being developed).
  setTime(1642968485);
  setHour = hour();
  setMin = minute();
  setSec = second();
  lcd.print("Time Setup: ");
  lcd.setCursor(0, 2);
  /*if(month() < 10){
    lcd.print("0");
    }*/
  setHour -= 12;
  lcd.print("0");
  lcd.print(setHour);
  lcd.print(":0");
  lcd.print(setMin);
  lcd.print(":0");
  lcd.print(setSec);
  if (hour() > 11) {
    lcd.print(" PM");
  }
  else {
    lcd.print(" AM");
  }
  lcd.setCursor(1, 2);
}


void adjMonth() { //increments and decrements MONTH
  if (analogRead(A3) > 400) { //increment
    setMonth++;
    lcd.print(setMonth);
    lcd.setCursor(1, 2);
    if (setMonth >= 9) {
      lcd.setCursor(0, 2);
    }
    if (setMonth > 12) {
      setMonth = 1;
      lcd.setCursor(0, 2);
      lcd.print("0");
      lcd.print(setMonth);
      lcd.setCursor(1, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    setMonth--;
    lcd.print(setMonth);
    lcd.setCursor(0, 2);
    if (setMonth < 1) {
      setMonth = 12;
      lcd.setCursor(0, 2);
      lcd.print(setMonth);
      lcd.setCursor(0, 2);
    }
    if (setMonth < 10) {
      lcd.setCursor(0, 2);
      lcd.print("0");
      if (setMonth <= 9) {
        lcd.print(setMonth);
        lcd.setCursor(1, 2);
      }
    }
  }
  delay(75);
}

void adjDay() {
  lcd.setCursor(3, 2);
  if (analogRead(A3) > 400) { //increment
    setDay++;
    if (setDay < 10) {
      lcd.setCursor(4, 2);
    }
    lcd.print(setDay);
    lcd.setCursor(3, 2);
    if (setDay > 31) {
      setDay = 1;
      lcd.setCursor(3, 2);
      lcd.print("0");
      lcd.print(setDay);
      lcd.setCursor(4, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    setDay--;
    if (setDay < 10) {
      lcd.print("0");
      lcd.setCursor(4, 2);
    }
    lcd.print(setDay);
    lcd.setCursor(3, 2);
    if (setDay < 1) {
      setDay = 31;
      lcd.setCursor(3, 2);
      lcd.print(setDay);
    }
  }
  delay(75);
}

void adjYear() {
  lcd.setCursor(6, 2);
  if (analogRead(A3) > 400) {
    setYear++;
    lcd.print(setYear);
    lcd.setCursor(6, 2);
  }
  delay(75);

  if (analogRead(A4) > 400) {
    setYear--;
    lcd.print(setYear);
    lcd.setCursor(6, 2);
  }
  delay(75);
}

void adjHour() {
  Serial.println(realHour);
  if (analogRead(A3) > 400) { //increment WORKS
    setHour++;
    realHour++;
    lcd.print(setHour);
    lcd.setCursor(1, 2);
    if (realHour <= 11) {
      lcd.setCursor(9, 2);
      lcd.print("AM");
      lcd.setCursor(1, 2);
    }
    else {
      lcd.setCursor(9, 2);
      lcd.print("PM");
      lcd.setCursor(1, 2);
    }
    if (setHour >= 9) {
      lcd.setCursor(0, 2);
    }
    if (setHour > 12) {
      setHour = 1;
      lcd.setCursor(0, 2);
      lcd.print("0");
      lcd.print(setHour);
      lcd.setCursor(1, 2);
    }
    if (realHour > 23) {
      realHour = 0;
      lcd.setCursor(9, 2);
      lcd.print("AM");
      lcd.setCursor(0, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    setHour--;
    realHour--;
    lcd.print(setHour);
    lcd.setCursor(0, 2);
    if (realHour <= 11) {
      lcd.setCursor(9, 2);
      lcd.print("AM");
      lcd.setCursor(0, 2);
    }
    else {
      lcd.setCursor(9, 2);
      lcd.print("PM");
      lcd.setCursor(0, 2);
    }
    if (setHour < 1) {
      setHour = 12;
      lcd.setCursor(0, 2);
      lcd.print(setHour);
      lcd.setCursor(0, 2);
    }
    if (realHour < 0) {
      realHour = 23;
      lcd.setCursor(9, 2);
      lcd.print("PM");
      lcd.setCursor(0, 2);

    }
    if (setHour < 10) {
      lcd.setCursor(0, 2);
      lcd.print("0");
      if (setHour <= 9) {
        lcd.print(setHour);
        lcd.setCursor(1, 2);
      }
    }
  }
  delay(75);
}

void adjMin() {
  lcd.setCursor(4, 2);
  if (analogRead(A3) > 400) { //sixth: set up minute
    setMin++;
    if (setMin > 9) {
      lcd.setCursor(3, 2);
    }
    lcd.print(setMin);
    lcd.setCursor(4, 2);
    if (setMin > 59) {
      setMin = 0;
      lcd.setCursor(3, 2);
      lcd.print("0");
      lcd.print(setMin);
      lcd.setCursor(4, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    setMin--;
    if (setMin < 1) {
      setMin = 59;
      //lcd.setCursor(3, 2);
      lcd.setCursor(3, 2);
    }

    if (setMin > 9) {
      lcd.setCursor(3, 2);
    }
    lcd.print(setMin);

    lcd.setCursor(4, 2);

    if (setMin < 10) {
      lcd.setCursor(3, 2);
      lcd.print("0");
      if (setMin <= 9) {
        lcd.print(setMin);
        lcd.setCursor(4, 2);
      }
    }
  }
  delay(75);
}

void adjSec() {
  lcd.setCursor(7, 2);
  if (analogRead(A3) > 400) { //sixth: set up minute
    setSec++;
    if (setSec > 9) {
      lcd.setCursor(6, 2);
    }
    lcd.print(setSec);
    lcd.setCursor(7, 2);
    if (setSec > 59) {
      setSec = 0;
      lcd.setCursor(6, 2);
      lcd.print("0");
      lcd.print(setSec);
      lcd.setCursor(7, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    setSec--;
    if (setSec < 1) {
      setSec = 59;
      lcd.setCursor(6, 2);
    }

    if (setSec > 9) {
      lcd.setCursor(6, 2);
    }
    lcd.print(setSec);

    lcd.setCursor(7, 2);

    if (setSec < 10) {
      lcd.setCursor(6, 2);
      lcd.print("0");
      if (setSec <= 9) {
        lcd.print(setSec);
        lcd.setCursor(7, 2);
      }
    }
  }
  delay(75);
}

void displayDateAndTime() {
  lcd.home();
  lcd.print("   ");
  int twelveHour = hour(); //for showing 12 hour time
  int militaryHour = hour(); //for AM/PM
  if (twelveHour > 12) { //displays 12 hour time
    twelveHour -= 12;
  }

  if (twelveHour == 0) {
    twelveHour = 12;
  }

  lcd.print(twelveHour);
  lcd.print(":");
  if (minute() < 10) {
    lcd.print("0");
  }
  lcd.print(minute());
  lcd.print(":");
  if (second() < 10) {
    lcd.print("0");
  }
  lcd.print(second());
  lcd.print(" ");

  if (militaryHour <= 11) { //change from AM -> PM and vice versa
    lcd.print("AM");
  }
  else {
    lcd.print("PM");
  }
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("   ");
  if (month() < 10) {
    lcd.print("0");
  }
  lcd.print(month());
  lcd.print("/");
  lcd.print(day());
  lcd.print("/");
  lcd.print(year());
}

void displayAlarm() {
  lcd.home();
  //lcd.blink();
  lcd.print(" Set alarm for: ");
  lcd.setCursor(0, 1);
  lcd.print("    ");

  if (alarmHour < 10) {
    lcd.print("0");
  }
  lcd.print(alarmHour);
  lcd.print(":");
  if (alarmMinute < 10) {
    lcd.print("0");
  }
  lcd.print(alarmMinute);
  if (alarmMilitaryHour <= 11) { //change from AM -> PM and vice versa
    lcd.print(" AM     ");
  }
  else {
    lcd.print(" PM     ");
  }
  if (alarmHour > 10) {
    lcd.setCursor(4, 2);
  }
  else {
    lcd.setCursor(5, 2);
  }
}


void adjAlarmHour() {
  if (analogRead(A3) > 400) { //increment WORKS
    alarmHour++;
    alarmMilitaryHour++;
    if (alarmHour == 10) {
      lcd.setCursor(4, 2);
    }
    lcd.print(alarmHour);
    lcd.setCursor(5, 2);
    if (alarmMilitaryHour <= 11) {
      lcd.setCursor(10, 2);
      lcd.print("AM");
      lcd.setCursor(5, 2);
    }
    else {
      lcd.setCursor(10, 2);
      lcd.print("PM");
      lcd.setCursor(5, 2);
    }
    if (alarmHour >= 9) {
      lcd.setCursor(4, 2);
    }
    if (alarmHour > 12) {
      alarmHour = 1;
      lcd.setCursor(4, 2);
      lcd.print("0");
      lcd.print(alarmHour);
      lcd.setCursor(5, 2);
    }
    if (alarmMilitaryHour > 23) {
      alarmMilitaryHour = 0;
      lcd.setCursor(10, 2);
      lcd.print("AM");
      lcd.setCursor(4, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    alarmHour--;
    alarmMilitaryHour--;
    lcd.print(alarmHour);
    lcd.setCursor(4, 2);
    if (alarmMilitaryHour <= 11) {
      lcd.setCursor(10, 2);
      lcd.print("AM");
      lcd.setCursor(4, 2);
    }
    else {
      lcd.setCursor(10, 2);
      lcd.print("PM");
      lcd.setCursor(4, 2);
    }
    if (alarmHour < 1) {
      alarmHour = 12;
      lcd.setCursor(4, 2);
      lcd.print(alarmHour);
      lcd.setCursor(4, 2);
    }
    if (alarmMilitaryHour < 0) {
      alarmMilitaryHour = 23;
      lcd.setCursor(10, 2);
      lcd.print("PM");
      lcd.setCursor(4, 2);

    }
    if (alarmHour < 10) {
      lcd.setCursor(4, 2);
      lcd.print("0");
      if (alarmHour <= 9) {
        lcd.print(alarmHour);
        lcd.setCursor(5, 2);
      }
    }
  }
  delay(75);
}

void adjAlarmMinute() {
  if (alarmMinute < 10) { //set cursor before minute is updated
    lcd.setCursor(8, 2);
  }
  else {
    lcd.setCursor(7, 2);
  }

  //lcd.setCursor(4, 2);
  if (analogRead(A3) > 400) { //sixth: set up minute
    alarmMinute++;
    if (alarmMinute > 9) {
      lcd.setCursor(7, 2);
    }
    lcd.print(alarmMinute);
    lcd.setCursor(8, 2);
    if (alarmMinute > 59) {
      alarmMinute = 0;
      lcd.setCursor(7, 2);
      lcd.print("0");
      lcd.print(alarmMinute);
      lcd.setCursor(8, 2);
    }
  }
  delay(75);

  if (analogRead(A4) > 400) { //decrement
    alarmMinute--;
    if (alarmMinute < 1) {
      alarmMinute = 59;
      //lcd.setCursor(7, 2);
      lcd.setCursor(7, 2);
    }

    if (alarmMinute > 9) {
      lcd.setCursor(7, 2);
    }
    lcd.print(alarmMinute);

    lcd.setCursor(8, 2);

    if (alarmMinute < 10) {
      lcd.setCursor(7, 2);
      lcd.print("0");
      if (alarmMinute <= 9) {
        lcd.print(alarmMinute);
        lcd.setCursor(8, 2);
      }
    }
  }
  delay(75);
}
void JamesBond() {

#include "pitches.h"
#define NO_SOUND 0 // make the rests in music


  //* 8-ohm speaker on digital pin 8


  //array of notes
  int melody[] = {
    /*NOTE_G4,NOTE_G4,NO_SOUND,NOTE_G4,NOTE_G4,NO_SOUND,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,
      NOTE_B3,NOTE_G3,NOTE_C4,NOTE_G3,NOTE_CS4,NOTE_G3,NOTE_C4,NOTE_G3,NOTE_B3,NOTE_G3,NOTE_C4,NOTE_G3,NOTE_CS4,NOTE_G3,NOTE_C4,NOTE_G3,
      NOTE_E4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_E4,NOTE_E4,NOTE_E4,
      NOTE_E4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_E4,NOTE_E4,NOTE_E4,*/

    NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_DS5, NOTE_D5, NOTE_B4, NOTE_A4, NOTE_B4,
    NOTE_E4, NOTE_G4, NOTE_DS5, NOTE_D5, NOTE_G4, NOTE_B4,
    NOTE_B4, NOTE_FS5, NOTE_F5, NOTE_B4, NOTE_D5, NOTE_AS5,
    NOTE_A5, NOTE_F5, NOTE_A5, NOTE_DS6, NOTE_D6, NO_SOUND
  };


  // note duration: 1 = whole note, 2 = half note, 4 = quarter note, 8 = eighth note, etc.
  int noteDurations[] = {
    /*8,8,2,8,8,2,16,8,16,8,8,
      2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,
      8,16,16,8,4,8,8,8,
      8,16,16,8,4,8,8,8,*/
    8, 16, 16, 8, 4, 8, 8, 8,
    8, 16, 16, 8, 4, 8, 8, 8,
    8, 16, 16, 8, 4, 8, 8, 8,
    8, 16, 16, 8, 4, 8, 8, 8,
    8, 2, 8, 8, 1,
    8, 4, 8, 4, 8, 8,
    8, 8, 4, 8, 4, 8,
    4, 8, 4, 8, 3
  };


  int pace = 1450; // change pace of music("speedy")


  for (int Note = 0; Note < 54; Note++) { //counter of Notes (54 limit the array)
    int duration = pace / noteDurations[Note]; //Adjust duration with the pace of music
    tone(13, melody[Note], duration); //Play note


    // to distinguish the notes, set a minimum time between them.
    delay(duration * 1.2);

  }
}
void timeSync() {
  lcd.noBlink();
  lcd.home();
  setTime(realHour, setMin, setSec, setDay, setMonth, setYear);
  militaryHour = hour();
  lcd.setCursor(0, 0);
  lcd.print("Press button for");
  lcd.setCursor(0, 1);
  lcd.print("date and time");
}
