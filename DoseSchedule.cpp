#include "DoseSchedule.h"
#include "DoseHead.h"


void DoseSchedule::turnPumpOn() {
	(pump).pumpOn();
	pump_is_running = true;
}

void DoseSchedule::turnPumpOff() {
	(pump).pumpOff();
	pump_is_running = false;
}

void DoseSchedule::runPump(int avol) {
	pump_start_time = millis();
	pump_run_time = ((pump).startDosingPump(avol));
	pump_is_running = true;
	container.take(avol);
}

boolean DoseSchedule::isPumpRunning() {
	return pump_is_running;
}

boolean DoseSchedule::isEnabled() {
	return enabled;
}

void DoseSchedule::setEnabled(boolean boo) {
	enabled = boo;
	if (!boo) {
		turnPumpOff();   // just in case the pump is running at the time.
	}
}

DoseSchedule::DoseSchedule() {
	setName("   ");
	reset_flag = false;
	booster_days = 0;
	booster_volume = 0;
	maxVolume = DEFAULT_MAXIMUM_DAILY_VOLUME;
	volume_dosed = 0;
	//rate = DEFAULT_RATE;
	pump_is_running = false;
	enabled = false;
	set_volume = 0;
	pump_run_time = 0;
	pump_start_time = 0;
	target_volume = 0;
	eeprom_location = 0;
}


void DoseSchedule::startupCode(byte apin, int aloc, char* aname) {
	pump.initPump(apin);
	turnPumpOff();
	eeprom_location = aloc;
	//name = aname;
	setName(aname);
	container.setName(name);
	if(!getSchedule()){
		setEnabled(false);
	}
	if(!getCal()){
		notCalibratedAlert.setActive(true, 2, name , F("NOT CALIBRATED"));
		setEnabled(false);
	}

}

char* DoseSchedule::getName() {
	return this->name;
}

void DoseSchedule::setName(char* aName) {
	strncpy(name , aName , 3);
	name[3] = 0;
}

void DoseSchedule::setDailyVolume(int vol) {
	set_volume = vol;
}

int DoseSchedule::getDailyVolume() {
	return set_volume;
}

void DoseSchedule::setCalibration(int mini, int maxi) {
	pump.minimum_flow_rate = mini;
	pump.maximum_flow_rate = maxi;
	saveCal(1);
	notCalibratedAlert.setActive(false);
}

unsigned long DoseSchedule::getPumpStartTime() {
	return pump_start_time;
}

unsigned long DoseSchedule::getPumpRunTime() {
	return pump_run_time;
}

TimeOfDay DoseSchedule::getStartTime() {
	return start_time;
}

TimeOfDay DoseSchedule::getEndTime() {
	return end_time;
}

TimeOfDay DoseSchedule::getInterval() {
	return interval;
}

int DoseSchedule::getMaxVolume(){
	return maxVolume;
}

void DoseSchedule::setMaxVolume(int aVol){
	maxVolume = aVol;
}


DoseContainer* DoseSchedule::getContainer() {
	return &container;
}

void DoseSchedule::setSchedule(TimeOfDay st, TimeOfDay en, TimeOfDay inter,
		int vol) {
	start_time = st;
	end_time = en;
	interval = inter;
	set_volume = vol;

	// Enforces a rule that there must be at least 1 interval between start and end times
	if ((((TimeOfDay::lengthOfTime(end_time, start_time) + MIDNIGHT) % MIDNIGHT))
			< interval.getTime()) {
		end_time.setTime(
				((start_time.getTime() - interval.getTime()) + MIDNIGHT)
						% MIDNIGHT);
	}

	initSchedule(); // initialize other variables in schedule to current point in time

}

void DoseSchedule::initSchedule() {
	volume_dosed = 0;  // reset for next calculation

	reset_flag = false;

	target_volume = set_volume;
	//  **TODO
	// If you set a new schedule while a booster is
	// scheduled, you will lose one day of booster
	// *****  NEED TO WORK OUT A FLAG TO FIX THIS

	//  Setup other variables to prepare for run
	//  Simulate that it was already running
	TimeOfDay init_time(now());

	if (isInRange(init_time)) {
		int set_point = (TimeOfDay::lengthOfTime(start_time, init_time)
				% MIDNIGHT);

		int set_point_doses = (set_point / interval.getTime()) + 1;

		volume_dosed = (calculateVolume(start_time, set_volume)
				* set_point_doses);

		last_time.setTime(
				(start_time.getTime()
						+ ((set_point_doses - 1) * interval.getTime()))
						% MIDNIGHT);
	} else {
		// Normal reset can handle it with the first dose if we're outside the window now
		// Just set up what it needs
		volume_dosed = 0;
		reset_flag = true;
		last_time = end_time;
	}
}


void DoseSchedule::resetSchedule() {
	target_volume = set_volume;
	if (booster_volume > 0)
		addBooster();   // This is where we pick up the booster

	if(target_volume > maxVolume){
		volExceedAlert.setActive(true, 2, name, F("Max Vol Exceed"));
	} else {
		// **TODO
		// The alert will only last one day.  We might fix this later.
		volExceedAlert.setActive(false);
	}

}

void DoseSchedule::runSchedule() {
	if (pump_is_running) {
		pumpTimer();
	} else if (enabled) {
		checkTimer();
	} // end if(enabled)
}


boolean DoseSchedule::checkTimer() {
	TimeOfDay run_time(now());   //  Get the time
	// This reset function will run once outside the dosing window
	if ((!(isInRange(run_time))) && (!reset_flag)) {
		volume_dosed = 0;  // reset tally
		reset_flag = true; // let the runner know the next dose is the first dose of the window
	}

	//  THIS IS THE CORE LOGIC
	if ((isInRange(run_time))
			&& (((TimeOfDay::lengthOfTime(last_time, run_time) + MIDNIGHT)
					% MIDNIGHT) >= interval.getTime())
			&& (volume_dosed < target_volume)) {
		// make a dose

		if (reset_flag == true) //  If this is the first dose of the schedule, then handle the target volume resets
		{
			resetSchedule();

			reset_flag = false;
		}

		int run_volume = calculateVolume(run_time,
				(target_volume - volume_dosed));  // get the volume

		volume_dosed += run_volume;  // update the tally
		last_time = run_time;   // set last_time to the current time

		runPump(run_volume);
		return true;
	}

	return false;
}

boolean DoseSchedule::pumpTimer() {
	//  TODO
	//  Change return value to whether pump running at end or not


	// Will return true if it turns the pump off
	// or return false if it leaves it running
	// but only as a flag.  The calling function
	// need not take any action from the schedules
	// point of view
	if (pump_is_running) {
		if ((millis() - pump_start_time) > pump_run_time) {
			turnPumpOff();
			pump_run_time = 0;
			pump_start_time = 0;

			return true;
		}
	}
	return false;
}

boolean DoseSchedule::isInRange(TimeOfDay _val) {
	//  Determines if the time of day argument is within the dosing window
	//  ie is between start and end times inclusive.

	if (start_time.getTime() < end_time.getTime()) {
		return ((_val.getTime() >= start_time.getTime())
				&& (_val.getTime() <= end_time.getTime()));  // simple case
	} else if (start_time.getTime() > end_time.getTime()) {
		return ((_val.getTime() >= start_time.getTime())
				|| (_val.getTime() <= end_time.getTime())); // accounts for midnight rollover
	} else
		return (1);   //  if start_time = end_time, then it's always in range.
}

int DoseSchedule::calculateVolume(TimeOfDay _time, int _vol) {
	//  Calculates the dose volume needed now to best finish the schedule

	int time_remain = TimeOfDay::lengthOfTime(_time, end_time);

	//  This if handles two different cases of identifying the last dose
	//  left in the old start == end stuff just in case
	if ((time_remain < interval.getTime())
			|| ((start_time.getTime() == end_time.getTime())
					&& (time_remain <= interval.getTime()))) {
		//  If this is the last dose, then dose everything that's left.
		return (_vol);
	} else {
		//  Calculate the volume needed to get there over time
		return ((_vol)
				/ ((((time_remain + MIDNIGHT) % MIDNIGHT) / interval.getTime())
						+ 1));
	}
}

void DoseSchedule::createBooster(int _vol, int _days) {
	//  puts booster amounts into the variables
	//  to let the program know a booster is
	//  requested.  booster_volume serves
	//  as it's own flag

	TimeOfDay boo_time(now());   //  Get the time
	booster_volume += _vol;

	if (_days > booster_days) {
		booster_days = _days;
	}

	int day1_volume;

	if ((isInRange(boo_time)) && (booster_days > 0)
			&& (TimeOfDay::lengthOfTime(boo_time, start_time)
					> (interval.getTime() * 2))) {
		//  If already inside window, calculate a dose for today and add to target
		//  Will not run if closer than 2 intervals to the next start of the schedule.

		day1_volume = (booster_volume / booster_days);

		if (booster_days > 1) {
			// If there's more than one day, break up the dose.
			int factor = (TimeOfDay::lengthOfTime(start_time, boo_time)
					/ TimeOfDay::lengthOfTime(boo_time, end_time));

			day1_volume = (day1_volume / (factor + 1));
		}

		booster_days -= 1;
		booster_volume -= day1_volume;

		target_volume += day1_volume;
	}
	//  If not in the window, then the call to addBooster with the first dose will handle everything.
}

void DoseSchedule::addBooster() {
	// This function adds a full days worth of booster dose to the target.
	// It should only be called at the beginning or at worst very early
	// in the schedule window.
	if (booster_days > 0)  //  booster days = 0 would break things
			{
		int day_volume = booster_volume / booster_days; // on day = 1 it should do everything left

		target_volume += day_volume; // add it to the days target, and the calculateVolume function handles the rest

		booster_volume -= day_volume;
		booster_days -= 1;
	} else {
		booster_volume = 0;  // If days == 0 then clear things out
	}
}

void DoseSchedule::singleDose(int aVolume) {
	runPump(aVolume);
}

void DoseSchedule::saveSchedule(int clr_flag) {
	// Saves the four user input variables to EEPROM

	byte flag;   // Indicates a saved schedule

	readFromEEPROM(eeprom_location + 18, flag);

	if (!clr_flag)  //  Pass a zero to clear EEPROM flag.
	{
		flag &= ~4;
		flag |= 3;
		flag |= 8;  // set disabled flag
		writeToEEPROM(eeprom_location + 18, flag);
		return;
	}

	else {
		int st = start_time.getTime();
		int et = end_time.getTime();
		int in = interval.getTime();
		int siz = container.getSize();

		flag |= 4;   // Indicates a saved schedule
		flag &=~3;   // Indicates a saved schedule
		if(enabled){
			flag &=~8;  // clear disabled flag
		} else {
			flag |=8;
		}
		writeToEEPROM(eeprom_location, st);
		writeToEEPROM(eeprom_location + 2, et);
		writeToEEPROM(eeprom_location + 4, in);
		writeToEEPROM(eeprom_location + 6, set_volume);
		//writeToEEPROM(eeprom_location + 8, rate);
		writeToEEPROM(eeprom_location + 10, maxVolume);
		writeToEEPROM(eeprom_location + 12, siz);
		writeToEEPROM(eeprom_location + 18, flag);
	}
}

boolean DoseSchedule::getSchedule() {
	//  Gets the four user input variables from EEPROM and rebuilds the class

	byte flag;

	readFromEEPROM(eeprom_location + 18, flag);
	if (!((flag & 7) == 4)) {  // 0 and 1 bit cleared and 2 bit set is a good schedule
		return false;           // It's not a good schedule
	} else {

		int st;
		int et;
		int in;
		int siz;

		readFromEEPROM(eeprom_location, st);
		readFromEEPROM(eeprom_location + 2, et);
		readFromEEPROM(eeprom_location + 4, in);
		readFromEEPROM(eeprom_location + 6, set_volume);
		//readFromEEPROM(eeprom_location + 8, rate);
		readFromEEPROM(eeprom_location + 10, maxVolume);
		readFromEEPROM(eeprom_location + 12, siz);

		start_time.setTime(st);
		end_time.setTime(et);
		interval.setTime(in);
		container.setSize(siz);

		setEnabled(!(flag & 8));

		initSchedule(); //  Reset the variables and simulate the last dose point.

		return true;
	}
}

void DoseSchedule::saveCal(int clr_flag) {
	byte flag;
	readFromEEPROM(eeprom_location + 18, flag);

	if (!clr_flag)  //  Pass a zero to clear calibration flag.
	{
		flag |= 48;
		writeToEEPROM(eeprom_location + 18, flag);
		return;
	}

	else {
		flag &= ~16;   // Indicates a saved calibration
		if (PWM_ENABLED){
			flag &= ~32;  // Indicates calibrated with PWM on.
		}
		writeToEEPROM(eeprom_location + 20, (pump).minimum_pwm_rate);
		writeToEEPROM(eeprom_location + 20 + 2, (pump).minimum_flow_rate);
		writeToEEPROM(eeprom_location + 20 + 6, (pump).maximum_flow_rate);
		writeToEEPROM(eeprom_location + 18, flag);
	}
}

boolean DoseSchedule::getCal() {

	byte flag;

	readFromEEPROM(eeprom_location + 18, flag);
	if (flag & 16) {
		return false;    // Not Calibrated Flag is set
	} else {
		readFromEEPROM(eeprom_location + 20, (pump).minimum_pwm_rate);
		readFromEEPROM(eeprom_location + 20 + 2, (pump).minimum_flow_rate);
		readFromEEPROM(eeprom_location + 20 + 6, (pump).maximum_flow_rate);

		return true;
	}

}

int DoseSchedule::isCal() {
	byte flag;

	readFromEEPROM(eeprom_location + 18, flag);
	if (flag & 16) {
		return 0;    // Pump not calibrated
	} else {
		if (flag & 32)
			return 1;  // Pump calibrated but not PWM
		else
			return 2;  // PWM calibrated
	}
}

