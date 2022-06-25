#define countof(a) (sizeof(a) / sizeof(a[0]))

#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(Wire); // include SOA and SCL

const char data[] = "Time is";

void setup () 
{
    Serial.begin(9600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    // Set time to system time
    Rtc.SetDateTime(compiled);
    Serial.println();

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
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 

/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    Rtc.SetMemory(0, 13);
    uint8_t written = Rtc.SetMemory(13, (const uint8_t*)data, sizeof(data) - 1); // remove the null terminator strings add
    Rtc.SetMemory(1, written);
/* end of comment out section */
}

void loop () 
{
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
    }

    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    delay(5000);

    // read data

    // get the offset we stored our data from address zero
    uint8_t address = Rtc.GetMemory(0);
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

        if (gotten != count ||
            count != sizeof(data) - 1) // remove the extra null terminator strings add
        {
            Serial.print("something didn't match, count = ");
            Serial.print(count, DEC);
            Serial.print(", gotten = ");
            Serial.print(gotten, DEC);
            Serial.println();
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

    
    delay(5000);
}



void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
