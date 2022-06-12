
int buttonState = 0;     


void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(2, INPUT);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(3, OUTPUT);

}

// the loop function runs over and over again forever
void loop() {
  buttonState = digitalRead(2);
  if (buttonState == HIGH) {
      // 2 HZ
      digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(500);                       // 500 ms
      digitalWrite(3, LOW);    // turn the LED off by making the voltage LOW
      delay(500);                       // 500 ms
    }
    else {
      // 1 HZ
      digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // 500 ms
      digitalWrite(3, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);                       // 500 ms
    }
    
    
}
