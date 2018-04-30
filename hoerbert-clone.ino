#include "main.h"

#include <ESP8266WiFi.h>

#if SD_DEBUG
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
#endif

void setup(void)
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("putting WiFi to sleep");
    WiFi.forceSleepBegin(); // turn off ESP8266 RF
    delay(1); // give RF section time to shutdown
    
    SD.begin(D0);

    // print root directory of SD for debugging
#ifdef SD_DEBUG
    File root = SD.open("/");
    printDirectory(root, 0);
#endif
    
    Serial.println("Setup finished");
}


de::ahans::Main main;

void loop()
{
    main.loop();
}
