// Pin 13 has an LED connected on most Arduino boards.
#define PIN_BLINKY_LED  13

#define LOOP_PERIOD     125


// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pin as an output.
	pinMode(PIN_BLINKY_LED, OUTPUT);
    Serial.begin(115200);
}


// the loop routine runs over and over again forever:
uint8_t count = 0;

void loop() { 
    digitalWrite(PIN_BLINKY_LED, HIGH);
    delay(LOOP_PERIOD);
    digitalWrite(PIN_BLINKY_LED, LOW);
    delay(LOOP_PERIOD);
    Serial.println(count, HEX);
    count++;
}
