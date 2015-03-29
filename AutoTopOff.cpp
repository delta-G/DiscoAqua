/*
 * AutoTopOff.cpp
 *
 *  Created on: Mar 29, 2015
 *      Author: David
 */

#include"AutoTopOff.h"

AutoTopOff::AutoTopOff() {

}

void AutoTopOff::initATO(Outlet apump, Outlet arefil, uint8_t shfp,
		uint8_t slfp, uint8_t rhfp, uint8_t rlfp) {
	pumpOutlet = apump;
	refilOutlet = arefil;
	/* All FLoats shall be assumed to be active LOW and active when LOW.
	 *     So a float that reads LOW means low water and a float that reads HIGH means high water
	 */
	floatPins[SUMP_HIGH] = shfp;
	floatPins[SUMP_LOW] = slfp;
	floatPins[RES_HIGH] = rhfp;
	floatPins[RES_LOW] = rlfp;

	for (int i = 0; i < 4; i++) {
		pinMode(floatPins[i], INPUT_PULLUP);
	}
}

void AutoTopOff::checkATO() {
	if (digitalRead(floatPins[SUMP_HIGH])) {
		pumpOutlet.turnOff();
	}
	if (!digitalRead(floatPins[SUMP_LOW])) {
		pumpOutlet.turnOn();
	}
}

void AutoTopOff::refilATO() {
	if (digitalRead(floatPins[SUMP_HIGH])) {
		pumpOutlet.turnOff();
	}
	if (!digitalRead(floatPins[SUMP_LOW])) {
		pumpOutlet.turnOn();
	}
}