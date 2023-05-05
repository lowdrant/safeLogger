/**
 * !! Example Code Only !!
 *
 * Creates logfiles of the form "data<digits>.csv" on an SD card with CS
 * connected to pin 4. Also has some Serial messages to make debugging easier.
 *
 */
#include "SD.h"
#include "safeLogger.h"
int SD_CS = 4;
String fn;
char fnbase[] = "data";
char fnsfx[] = "csv";

void setup() {
    Serial.begin(115200);
    while (!SD.begin(SD_CS)) {
        Serial.println(F("SD card error"));
        delay(300);
    }
    File root = SD.open(F("/"));
    if (!root) {
        Serial.println(F("Error opening root"));
    }
    fn = getLogName(&root, fnbase, fnsfx);
    root.close();
    File logFile = SD.open(fn, FILE_WRITE);
    if (logFile) {
        logFile.println(F("I am a file"));
        logFile.close();
    } else {
        Serial.println(F("Logfile error"));
    }
}

void loop() {
    Serial.print(F("fn="));
    Serial.println(fn);
    delay(300);
}
