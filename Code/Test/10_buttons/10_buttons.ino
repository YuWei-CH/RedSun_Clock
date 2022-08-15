// Pin 13 has an LED connected on most Arduino boards.
#define PIN_BLINKY_LED  13

// pins for random clock.  change for your clock!
#define PIN_BUTTON_0    2
#define PIN_BUTTON_1    3
#define PIN_BUTTON_2    15

#define LOOP_PERIOD     64


// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pin as an output.
  pinMode(PIN_BLINKY_LED, OUTPUT);
  pinMode(PIN_BUTTON_0, INPUT);
  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);
  Serial.begin(115200);
}


// the loop routine runs over and over again forever:
void loop() { 
    digitalWrite(PIN_BLINKY_LED, HIGH);
    delay(LOOP_PERIOD);
    digitalWrite(PIN_BLINKY_LED, LOW);
    delay(LOOP_PERIOD);
    Serial.print("buttons: ");
    if(!digitalRead(PIN_BUTTON_0)) {
      Serial.print("B0 ");
    } else {
      Serial.print("   ");
    }
    if(!digitalRead(PIN_BUTTON_1)) {
      Serial.print("B1 ");
    } else {
      Serial.print("   ");
    }
    if(!digitalRead(PIN_BUTTON_2)) {
      Serial.print("B2");
    } else {
      Serial.print("  ");
    }
    Serial.println("");
}
