# safeLogger
Crash-resistant SD log file creation library for Arduino

## The Problem
I want to log data to an SD card on an Arduino. What happens if the Arduino
resets, or my log file already exists? Do I overwrite? How do I create a new
filename? Does appending to the previous filename make sense?

I don't want to risk file overwrites or lost data, and I didn't want to deal
with weird timestamps in my datafiles, so I created this library.

## Quickstart
Clone this repo into your Arduino project, and then include it with
`#include "safeLogger/safeLogger.h"`. Determine a basename and suffix for
your logfile, e.g. "log" and "csv". Then you can get the "safe" filename using
`getLogName(&dir, "data", "csv");`

### Requirements
* "SD.h", which should come pre-installed in the Arduino IDE.

### Code Snippet
This creates a logfile in the "root" of the SD card.
```
#include "SD.h"
#include "safeLogger/safeLogger.h"
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
```
### How it Works
For a file suffix (e.g. "csv") and a basename (e.g. "data"), it will find all
the files named "data<digits>.csv", then return a filename
"data<max(digits)+1>.csv". This file is guaranteed to not exist and preserves
sensible filename ordering.

Now everytime my Arduino resets, it creates a new file with digits+1, and I
know if/when it reset and don't lose any data in the process, all while keeping
my datafiles nice and ordered.
