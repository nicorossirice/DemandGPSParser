#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 8
// Connect the GPS RX (receive) pin to Digital 7

// you can change the pin numbers to match your wiring:
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

void setup()
{

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic parsing test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  // GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);   // 5 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
  // Example of delayed parsing of GPS output
  Serial.println("Starting loop...");
  delay(3000);
  Serial.println("Getting location...");
  if (update_loc())
  {
    Serial.println("Sucessful update");
    Serial.print("Angle: ");
    Serial.println(GPS.angle);
    Serial.print("Latitude: ");
    Serial.println(GPS.latitude, 4);
    Serial.print("Longitude: ");
    Serial.println(GPS.longitude, 4);
  }
  else
  {
    Serial.println("Update timed out");
  }
  Serial.println("\n\n");
  delay(3000);
}

// Parses the input in order to update the values stored in GPS.
// If more than 2 seconds pass without a valid location, the function will timeout.
// On success returns true, false otherwise.
bool update_loc()
{
  uint32_t start = millis();
  while (millis() - start < 2000)
  {
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if ((c) && (GPSECHO))
      Serial.write(c);

    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {
      // a tricky thing here is if we print the NMEA sentence, or data
      // we end up not listening and catching other sentences!
      // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
      //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

      if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
        continue;  // we can fail to parse a sentence in which case we should just wait for another
      else
      {
        if (GPS.fix) {
          if (GPSECHO) {
            Serial.print("\nTime: ");
            if (GPS.hour < 10) { Serial.print('0'); }
            Serial.print(GPS.hour, DEC); Serial.print(':');
            if (GPS.minute < 10) { Serial.print('0'); }
            Serial.print(GPS.minute, DEC); Serial.print(':');
            if (GPS.seconds < 10) { Serial.print('0'); }
            Serial.print(GPS.seconds, DEC); Serial.print('.');
            if (GPS.milliseconds < 10) {
              Serial.print("00");
            } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
              Serial.print("0");
            }
            Serial.println(GPS.milliseconds);
            Serial.print("Date: ");
            Serial.print(GPS.day, DEC); Serial.print('/');
            Serial.print(GPS.month, DEC); Serial.print("/20");
            Serial.println(GPS.year, DEC);
            Serial.print("Fix: "); Serial.print((int)GPS.fix);
            Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
            Serial.print("Location: ");
            Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
            Serial.print(", ");
            Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

            Serial.print("Speed (knots): "); Serial.println(GPS.speed);
            Serial.print("Angle: "); Serial.println(GPS.angle);
            Serial.print("Altitude: "); Serial.println(GPS.altitude);
            Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
            Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
          }
          
          return true;
        }
      }
    }
  }
  return false;
}
