int buttonState = 0;     


void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(2, INPUT);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(3, OUTPUT);
  // open the serial port at 9600 bps:
  Serial.begin(9600);

}

// the loop function runs over and over again forever
void loop() {
  buttonState = digitalRead(2);
  if (buttonState == HIGH) {
      // 2 HZ
      digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(10);                       // 500 ms
      Serial.println("Button Presssed");
    }
    digitalWrite(3, LOW);
    delay(1000);
}
