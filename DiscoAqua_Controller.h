// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef DiscoAqua_Controller_H_
#define DiscoAqua_Controller_H_
#include "Arduino.h"
//add your includes for the project DiscoAqua_Controller here
#include "DosingPump.h"
#include "Time.h"
#include "TimeOfDay.h"
#include "LiquidCrystal_SPI_8Bit.h"
#include "DoseSchedule.h"
#include "DoseHead.h"
#include "DiscoAlert.h"
#include "DS3234RTC.h"
#include <avr/wdt.h>

//end of add your includes here


#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project DiscoAqua_Controller here

void heartbeat();


//Do not add code below this line
#endif /* DiscoAqua_Controller_H_ */
