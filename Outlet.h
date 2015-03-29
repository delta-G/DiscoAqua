/*
 * Outlet.h
 *
 *  Created on: Mar 28, 2015
 *      Author: David
 */

#ifndef OUTLET_H_
#define OUTLET_H_

#include "PowerLine.h"


class Outlet {

public:

	enum OutletStates {OFF, ON, UNKNOWN};

	uint8_t houseCode;
	uint8_t unitCode;

	OutletStates currentState;

	Outlet();
	Outlet(uint8_t, uint8_t);

	OutletStates getCurrentState();

	void turnOn();
	void turnOff();
	void resend();







};




#endif /* OUTLET_H_ */
