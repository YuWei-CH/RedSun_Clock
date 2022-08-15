#include <LedControl.h>
#include <Wire.h>
#include <RtcDS1307.h>
#include <Servo.h>
#include <EEPROM.h>

// example RTC code modified from :
// https://github.com/Makuna/Rtc/blob/master/examples/DS1307_Simple/DS1307_Simple.ino

// LED Display Code from:
// https://playground.arduino.cc/Main/LedControl/

// Pin 13 has an LED connected on most Arduino boards, including this clock
#define PIN_BLINKY_LED    13

// System constants
#define LOOP_PERIOD       10
#define COLON_BLINK_TIME (100 / LOOP_PERIOD)

// LED Controller pins
#define PIN_LED_CONTROLLER_DATA   10
#define PIN_LED_CONTROLLER_CLK    9
#define PIN_LED_CONTROLLER_LOAD   8

#define LED_BRIGHTNESS_MIN        0
#define LED_BRIGHTNESS_MAX        16
#define LED_BRIGHTNESS_DEFAULT    6

// Light sensor photo resistor
#define PIN_LIGHT_SENSOR            A0

#define LIGHT_SENSOR_AMBIENT        950
#define LIGHT_SENSOR_GAIN           100

#define LIGHT_SENSOR_LPF            100.0


// Light sensor
float light_sensor_lpf = LIGHT_SENSOR_AMBIENT;

void light_sensor_update() {
  int light_sensor_val = analogRead(PIN_LIGHT_SENSOR);
  light_sensor_lpf = ((float)light_sensor_val + (light_sensor_lpf * (LIGHT_SENSOR_LPF - 1.0))) / LIGHT_SENSOR_LPF;
}

int brightness_adjust() {
  // compute the light sensor brightness adjustment
  float brightness_adjust;
  brightness_adjust = (light_sensor_lpf - (float)(LIGHT_SENSOR_AMBIENT)) / (float)(LIGHT_SENSOR_GAIN) ;
  return (int)brightness_adjust;
}


// Create a new LedControl object.
// We use pins 8, 9 and 10 on the Arduino for the SPI interface
// Pin 10 is connected to the DATA IN-pin of the first MAX72XX
// Pin 9 is connected to the CLK-pin of the first MAX72XX
// Pin 8 is connected to the LOAD(/CS)-pin of the first MAX72XX
// There will only be a single MAX72XX attached to the arduino
LedControl display = LedControl(PIN_LED_CONTROLLER_DATA, PIN_LED_CONTROLLER_CLK,
                                PIN_LED_CONTROLLER_LOAD, 1);

// RTC setup.  We use a DS1307 RTC
RtcDS1307<TwoWire> Rtc(Wire);

// Maps the day IDs 0-6 to human readable day strings starting with Sunday == 0.
const char *DayOfWeekString(int DoW) {
  switch (DoW) {
  case 0:
    return "Sun";
  case 1:
    return "Mon";
  case 2:
    return "Tue";
  case 3:
    return "Wed";
  case 4:
    return "Thu";
  case 5:
    return "Fri";
  case 6:
    return "Sat";
  default:
    return "---";
  }
}

// Formats and writes the given time to the LED display.
void DisplayTime(const RtcDateTime *time) {
  // read the current time
  uint8_t hour = time->Hour();
  uint8_t minute = time->Minute();

  // handle 12-hour time silliness
  if (hour == 0) {
    hour = 12;
  }
  if (hour > 12) {
    hour = hour - 12;
  }
  // Hide the hour 10s place if the hour is less than 10
  if (hour >= 10) {
    display.setDigit(0, 0, 1, false);
  } else {
    display.setChar(0, 0, ' ', false);
  }
  display.setDigit(0, 1, hour % 10, false);
  display.setDigit(0, 2, minute / 10, false);
  display.setDigit(0, 3, minute % 10, false);
}


// Formats and prints the given time to the UART.
void PrintTime(const RtcDateTime *time) {
  uint8_t hour = time->Hour();
  // handle 12-hour time silliness
  boolean pm = false;
  if (hour == 0) {
    hour = 12;
  } else if (hour > 12) {
    hour -= 12;
    pm = true;
  }
  uint8_t minute = time->Minute();
  uint8_t second = time->Second();

  // format a descriptive time/date output with DOW and 12-hour format
  Serial.print(DayOfWeekString(time->DayOfWeek()));
  Serial.print(" ");
  Serial.print(time->Year(), DEC);
  Serial.print("-");
  Serial.print(time->Month(), DEC);
  Serial.print("-");
  Serial.print(time->Day(), DEC);
  Serial.print(" ");
  Serial.print(hour, DEC);
  Serial.print(":");
  if (minute < 10) {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second < 10) {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  if (pm) {
    Serial.print(" pm");
  } else {
    Serial.print(" am");
  }
  Serial.print("  light:");
  //Serial.print(analogRead(PIN_LIGHT_SENSOR), DEC);
  //Serial.print(",");
  Serial.print(light_sensor_lpf, 1);
  Serial.print(",");
  Serial.print(brightness_adjust());

  Serial.println("");
}


// The Arduino setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(PIN_BLINKY_LED, OUTPUT);

  // setup setial port
  Serial.begin(115200);

  // Start the I2C interface
  Wire.begin();
  Rtc.Begin();

  // Setup and check the time on the RTC clock
  RtcDateTime time_compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) {
    if (Rtc.LastError() != 0) {
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for
      // what the number means
      Serial.print("RTC communications error = ");
      Serial.println(Rtc.LastError());
    } else {
      // Common Causes:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");
      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(time_compiled);
    }
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  Serial.print("Compile time=");
  Serial.print(uint32_t(time_compiled), DEC);
  Serial.print(" :");
  PrintTime(&time_compiled);

  RtcDateTime time_now = Rtc.GetDateTime();
  Serial.print("    RTC time=");
  Serial.print(uint32_t(time_now), DEC);
  Serial.print(" :");
  PrintTime(&time_now);

  // figure out if we should keep the RTC time or take the compiled time
  if (uint32_t(time_now) < uint32_t(time_compiled)) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(time_compiled);
  } else if (uint32_t(time_now) > uint32_t(time_compiled)) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (uint32_t(time_now) == uint32_t(time_compiled)) {
    Serial.println(
        "RTC is the same as compile time! (not expected but all is fine)");
  }
  // uncomment this line to force the RTC to take the compiled time
  //Rtc.SetDateTime(time_compiled);

  // disable the square wave output
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);

  // wake up the MAX72XX from power-saving mode
  display.shutdown(0, false);
  display.setIntensity(0, LED_BRIGHTNESS_DEFAULT);

}


// State used in the loop, which must be global because of the way arduino
// exposes the loop() function. Rather than declaring this on the stack in main
// we make it global so the values persist between loop iterations.
uint8_t second_old = 0;
uint16_t colon_blink_counter = 0;

// Arduino provides a main() function that will repeatedly call loop() in a loop
// as fast as possible. We can fill loop() with code that we want to run
// periodically, and then provide rate limiting so that it runs at a regular
// period.
void loop() {
  // Update the light sensor.
  light_sensor_update();

  // Check for display brightness updates, and commit changes to EEPROM.
  RtcDateTime now = Rtc.GetDateTime();
  //display.setIntensity(0, constrain(LED_BRIGHTNESS_DEFAULT + brightness_adjust(), LED_BRIGHTNESS_MIN, LED_BRIGHTNESS_MAX));
  display.setIntensity(0, constrain(LED_BRIGHTNESS_DEFAULT, LED_BRIGHTNESS_MIN, LED_BRIGHTNESS_MAX));

  // Check the time, and whenever a second has passed, blink the colon and
  // ensure the time is up to date.
  if (now.Second() != second_old) {
    DisplayTime(&now);
    PrintTime(&now);
    colon_blink_counter = COLON_BLINK_TIME;
  }
  second_old = now.Second();

  // Count down some number of loop ticks before turning the colon back off.
  // It's turned back on every second by the code above.
  if (colon_blink_counter > 0) {
    digitalWrite(PIN_BLINKY_LED, HIGH);
    display.setChar(0, 4, ' ', true);
    colon_blink_counter--;
  } else {
    digitalWrite(PIN_BLINKY_LED, LOW);
    display.setChar(0, 4, ' ', false);
  }
  // Rate limit the loop
  delay(LOOP_PERIOD);
}
