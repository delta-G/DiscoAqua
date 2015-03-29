/*
 * Outlet.cpp
 *
 *  Created on: Mar 28, 2015
 *      Author: David
 */

#include "Outlet.h"



Outlet::Outlet(){
	houseCode = 0;
	unitCode = 0;
	currentState = OFF;

}

Outlet::Outlet(uint8_t aHouse, uint8_t aUnit){
	houseCode = aHouse;
	unitCode = aUnit;
	currentState = OFF;
}

Outlet::OutletStates Outlet::getCurrentState(){
	return currentState;
}

void Outlet::turnOn(){
	if(PowerLine.freeSpace()){
		PowerLine.sendCommand(houseCode, unitCode, UNIT_ON);
		currentState = ON;
	}
}

void Outlet::turnOff(){
	if(PowerLine.freeSpace()){
			PowerLine.sendCommand(houseCode, unitCode, UNIT_OFF);
			currentState = OFF;
		}
}

void Outlet::resend(){
	if(currentState == ON){
		turnOn();
	}
	else{
		turnOff();
	}
}
