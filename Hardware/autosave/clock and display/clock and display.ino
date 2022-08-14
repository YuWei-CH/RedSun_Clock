// Including the required Arduino dot matrix libraries
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


// Including the required DS1307 libraries
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(Wire); // include SOA and SCL


// Define for hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW


// Define size, and output pins for RTC
#define MAX_DEVICES 4
#define CS_PIN 8
#define DATA_PIN 10
#define CLK_PIN 9


// Define for global variable
#define countof(a) (sizeof(a) / sizeof(a[0])) // method of count time
const char data[] = "Time is"; // Time display area(on serial)
  

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Define Button var
int Sel = 2;
int Up = 3;
int Down = 4;
int dateButtonState = 0;
int secondButtonState = 0;

// Define LED var
int LED = 13;

// Define joystick var
int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;
int VRx = A1;
int VRy = A2;
int SW = 6;

// define buzzer
int buzzer = 5;

void setup() {

  // initialize digital pin LED as an output.
  pinMode(LED, OUTPUT);
  
  // initialize the pushbutton pin as an input:
  pinMode(Sel, INPUT); // Sel & DateTime (M/Y/D)
  pinMode(Up, INPUT); // UP & DayTime (H/M)
  pinMode(Down, INPUT); // Down & Second (S)
  
  // define the brightness
  int bright_value = analogRead(A0);
  Serial.println(bright_value);

  // define the Joystick for alarm
  // var for joystcik controlled alarm
  pinMode(VRx, INPUT);  // X : > 400 Left; 0 - 50 stay; < -400 right
  pinMode(VRy, INPUT);  // Y : < -400 UP; 0 - 50 stay; > 400 down
  pinMode(SW, INPUT_PULLUP); // SW: 0 push;

  // define the buzzer
  pinMode(buzzer, OUTPUT);

  

    // Initialize the Serial for time display
    Serial.begin(9600);
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    // Initialize RTC object
    Rtc.Begin();

    // Define the RTC initial time
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printTime(compiled);

    // Set time to system time and print it
    Rtc.SetDateTime(compiled);
    Serial.println();

    // Error check for RTC
     // Time invalid case
     if (!Rtc.IsDateTimeValid()) 
      {
        if (Rtc.LastError() != 0)
        {
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            Serial.println("RTC lost confidence in the DateTime!");
            Rtc.SetDateTime(compiled);
        }
         // Red LED alert
        for (int i = 0; i < 3; ++i)
        {
        digitalWrite(13, HIGH);
        delay(500);
        digitalWrite(13, LOW);
        }
      }

        // RTC isn't running case
        if (!Rtc.GetIsRunning())
          {
            Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
            // Red LED alert
            for (int i = 0; i < 3; ++i)
            {
            digitalWrite(13, HIGH);
            delay(500);
            digitalWrite(13, LOW);
            }
          }
      
      
      // Initialize current time
      RtcDateTime now = Rtc.GetDateTime();
      if (now < compiled) // make sure the compile time is older than RTC
        {
            Serial.println("RTC is older than compile time!  (Updating DateTime)");
            Rtc.SetDateTime(compiled);
        }

     // Set up the RTC Pin
      // never assume the Rtc was last configured by you, so
      // just clear them to your needed state
      Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);

    // Time memory
     /* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    Rtc.SetMemory(0, 13);
    uint8_t written = Rtc.SetMemory(13, (const uint8_t*)data, sizeof(data) - 1); // remove the null terminator strings add
    Rtc.SetMemory(1, written);
    /* end of comment out section */
      
 
  // Intialize the object
  myDisplay.begin();

  // Set the intensity (brightness) of the display (0-15)
  if (bright_value > 700){
     myDisplay.setIntensity(0); // brighter in day
  }
  else {
    myDisplay.setIntensity(10); // darker in night
  }

  // Clear the display
  myDisplay.displayClear();
  
}

void loop() {

  // RTC error check
  if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
        // Red LED alert
        for (int i = 0; i < 3; ++i)
        {
        digitalWrite(13, HIGH);
        delay(500);
        digitalWrite(13, LOW);
        }
    }

    // Set up current time
    RtcDateTime now = Rtc.GetDateTime();
    //Print time for check
    printTime(now);
    Serial.println();
    
    // ________
    //delay(5000);

    // get the offset we stored our data from address zero
    uint8_t address = Rtc.GetMemory(0);
    // Address check
    if (address != 13)
    {
        Serial.println("address didn't match");
    }
    else
    {
        // get the size of the data from address 1
        uint8_t count = Rtc.GetMemory(1);
        uint8_t buff[20];
       
        // get our data from the address with the given size
        uint8_t gotten = Rtc.GetMemory(address, buff, count);
        
        // remove the extra null terminator strings add
        if (gotten != count || count != sizeof(data) - 1) 
        {
            Serial.print("something didn't match, count = ");
            Serial.print(count, DEC);
            Serial.print(", gotten = ");
            Serial.print(gotten, DEC);
            Serial.println();
            // Red LED alert
            for (int i = 0; i < 3; ++i)
            {
            digitalWrite(13, HIGH);
            delay(500);
            digitalWrite(13, LOW);
            }
        }
        Serial.print("data read (");
        Serial.print(gotten);
        Serial.print(") = \"");
        for (uint8_t ch = 0; ch < gotten; ch++)
        {
            Serial.print((char)buff[ch]);
        }
        Serial.println("\"");
    }
    //delay(5000);

    
dateButtonState = digitalRead(Up);
secondButtonState = digitalRead(Down);
printDayTime(now,myDisplay);
if (dateButtonState == LOW) {
  printDateTime(now,myDisplay);
}
else if (secondButtonState == LOW) {
  printSeconds(now,myDisplay);
}

// Setting alarm
SW_state = digitalRead(SW);
mapX = map(xPosition, 0, 1023, -512, 512);
mapY = map(yPosition, 0, 1023, -512, 512);
int alarm_H = 0;
int alarm_M = 0;
bool isAlarm = false;
if (SW_state == 0) { // SW been pressed, begin setting
  isAlarm = true;
}

int theAlarm[] = {0,0}; // save the alarm time
int pointer = 0; // save current place of alarm array
while (isAlarm){ // begin setting
  digitalWrite(13, HIGH); // light up
  xPosition = analogRead(VRx); // left or right
  yPosition = analogRead(VRy); // up or down
  myDisplay.setTextAlignment(PA_LEFT);
  myDisplay.print(theAlarm[0] + "." + theAlarm[1]);
  if (xPosition > 400 or xPosition < -400) { // change x direction, hour or mintues
    if (pointer == 0 ){
      pointer = 1; 
    }
    else {
      pointer = 0;
    }
  }
  if (yPosition < -400){ // go up
    if (pointer == 0 ) { //  hour seeting
      if (theAlarm[pointer] < 23 and theAlarm[pointer] > -1) // between 0 and 23
      { 
        theAlarm[pointer] ++;
      }
      else 
      {
        theAlarm[pointer] = 0;
      }
   }
   else { //  minutes setting
    if (theAlarm[pointer] < 59 and theAlarm[pointer] > -1) // between 0 and 59
      { 
        theAlarm[pointer] ++;
      }
      else 
      {
        theAlarm[pointer] = 0;
      }
   }
   
  }
  if (yPosition > 400) { // go down
    if (pointer == 0 ) { //  hour seeting
      if (theAlarm[pointer] < 24 and theAlarm[pointer] > 0) // between 1 and 24
      { 
        theAlarm[pointer] --;
      }
      else 
      {
        theAlarm[pointer] = 0;
      }
   }
   else { // minutes setting
    if (theAlarm[pointer] < 60 and theAlarm[pointer] > 1) // between 1 and 60
      { 
        theAlarm[pointer] --;
      }
      else 
      {
        theAlarm[pointer] = 0;
      }
  }
 }
  if (SW_state == 0) { // SW been pressed, end setting
    isAlarm = false;
  }
digitalWrite(13, LOW);
pointer = 0;
delay(500);
}


// Alarm check
if (now.Hour() == theAlarm[0] and now.Minute() == theAlarm[1]) {
tone(buzzer, 1000); // Send 1KHz sound signal
delay(4000);        // sound for 4 sec
noTone(buzzer);     // Stop sound...
delay(500);
}

// loop ending
delay(1000);
}

// General time display through Serial
void printTime(const RtcDateTime& dt)
{
    char timestring[20];
    snprintf_P(timestring, 
            countof(timestring),
            PSTR("%02u/%02u/%04u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(timestring);
}

// Display date time, like Month, Day
void printDateTime(const RtcDateTime& dt, const MD_Parola& myDisplay)
{
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u"),
            dt.Month(),
            dt.Day() );
    Serial.print(datestring);
    myDisplay.displayClear();
    myDisplay.setTextAlignment(PA_CENTER);
    myDisplay.print(datestring); 
}

// Display hours,minutes
void printDayTime(const RtcDateTime& dt, const MD_Parola& myDisplay)
{
    char daystring[20];
    snprintf_P(daystring, 
            countof(daystring),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute());
    Serial.print(daystring);
    myDisplay.displayClear();
    myDisplay.setTextAlignment(PA_CENTER);
    myDisplay.print(daystring);
}

// Display seconds
void printSeconds(const RtcDateTime& dt, const MD_Parola& myDisplay)
{
    char Seconds[20];
    snprintf_P(Seconds, 
            countof(Seconds),
            PSTR("Sec:%02u"),
            dt.Second());
    Serial.print(Seconds);
    myDisplay.displayClear();
    myDisplay.setTextAlignment(PA_LEFT);
    myDisplay.print(Seconds);
}
