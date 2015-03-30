/*
 * AutoTopOff.h
 *
 *  Created on: Mar 29, 2015
 *      Author: David
 */

#ifndef AUTOTOPOFF_H_
#define AUTOTOPOFF_H_

#include"Arduino.h"
#include"Outlet.h"

class AutoTopOff {

	enum pinNames {SUMP_HIGH, SUMP_LOW, RES_HIGH, RES_LOW};

public:

	Outlet pumpOutlet;
	Outlet refilOutlet;
	uint8_t floatPins[4];

	// Times in seconds
	unsigned int minOnTime;
	unsigned int minOffTime;
	unsigned int maxOnTime;


	AutoTopOff();
	void initATO(Outlet, Outlet, uint8_t, uint8_t, uint8_t, uint8_t);

	void checkATO();

	void refilATO();


};

#endif /* AUTOTOPOFF_H_ */
