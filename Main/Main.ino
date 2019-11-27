/*
September 2019

Fritz Keyzer
fritzkeyzer@gmail.com

For Carel Kostense
carel@waterworksautomation.co.za
www.waterworksautomation.co.za
*/

/*

Scheduler v1 running on Adafruit Itsy Bitsy M0

Arduino IDE 1.8.5

*/

//builtin libraries
#include <stdio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//custom libraries - also copied to libraries folder
#include "Adafruit_MCP23017.h"
#include "Adafruit_DotStar.h"
#include "RTClib.h"
#include "SimpleThread.h"
#include "LiquidCrystal_I2C.h"
//#include "FlowSensor.h"
#include <IniFile.h>
//#include <Adafruit_SPIFlash.h>

//settings
SimpleThread fastTimer(20);						//input and output states 
SimpleThread mediumTimer(1000);					//control logic
SimpleThread timerSDCheck(10000);				//sd card check interval

long sdCheck_good = 60000;
long sdCheck_error = 10000;

//times
DateTime time_now;
//DateTime time_init;
DateTime time_logErrorSince;

//configurable settings
int setting_scheduleStartHour = 8;
int setting_scheduleStartMinute = 00;
int setting_scheduleMinutesPerZoneLight = 10;
int setting_scheduleMinutesPerZoneHeavy = 20;
int setting_seedlingInterval = 60;
int setting_seedlingTime = 5;
float setting_flowVolumePerTick = 0.5;

//states
int state_sequencePos = -1;
bool state_heavyDay = true;
bool state_valves[8];
bool state_manualPower = false;
int state_manualZone = 0;
bool state_seedlingWatering = false;

bool state_buttonA = false;
bool state_buttonB = false;
bool state_buttonC = false;
bool state_buttonD = false;

int state_soilMoisture = 0;
float state_pressure = 0;
//float state_flowRate = 0;
float state_flowVolume = 0;


//error states
bool error_SD = false;


void setup()
{
	Serial.begin(115200);
	
	//while (!Serial);
	
	Serial.println("Serial Connected. Hello");
	Serial.println("Scheduler v1");
	
	
	clock_setup();
	io_setup();
	control_setup();
	display_setup();
	sd_setup();
	sd_readSettings();
	
	Serial.println("Setup complete");
	
	sd_writeSettings();
}



void loop()
{
	if (fastTimer.check())
	{
		//output set			- set output pins
		io_output_update();
		
		//input
		io_input_update();

		//heartbeat LED (DotStar)
		io_heartbeatLED_update();
		
		//display
		display_update();
		
		//control scheme		- determine whether pump should be pumping or not
		control_update();
		
		//event log				- log events to sd card, set flags equal to states
		event_update();
	}
	
	if (mediumTimer.check())
	{
		//time update			- update current time, update from rtc once per day
		clock_update();
	}

	if (timerSDCheck.check())
	{
		Serial.println("Checking SD Card");
		sd_checkSDCard();
	}
}



