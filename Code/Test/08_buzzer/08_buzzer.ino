/*
Adafruit Arduino - Lesson 10. Simple Sounds
*/

#define PIN_BLINKY_LED  13
#define PIN_SPEAKER     7

//int numTones = 10;
//int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
////            mid C  C#   D    D#   E    F    F#   G    G#   A

int numTones = 7;
int tones[] = {261, 294, 330, 349, 392, 440, 512};
//            mid C  D    E    F    G    A    C

void setup()
{
  pinMode(PIN_BLINKY_LED, OUTPUT);
}

int ledState = 1;
void loop()
{
  for (int i = 0; i < numTones; i++)
  {
    digitalWrite(PIN_BLINKY_LED, ledState);
    tone(PIN_SPEAKER, tones[i]);
    delay(500);
    ledState = 1 - ledState;
  }
  noTone(PIN_SPEAKER);
}
