#include <Arduino.h>
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Main.ino"
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Main.ino"
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


#line 81 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Main.ino"
void setup();
#line 105 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Main.ino"
void loop();
#line 11 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\ClockManager.ino"
void clock_setup();
#line 37 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\ClockManager.ino"
void clock_update();
#line 42 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\ClockManager.ino"
void clock_set(int year, int month, int day, int hour, int minute);
#line 9 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
void control_setup();
#line 14 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
void control_update();
#line 115 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
bool control_checkTime();
#line 128 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
void control_resetValveStates();
#line 136 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
void control_resetSeedlingValve();
#line 141 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
void control_manualZoneIncrement();
#line 150 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
void control_manualPowerToggle();
#line 36 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_setup();
#line 51 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_update();
#line 88 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_cycle();
#line 154 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_event(String event, String value);
#line 171 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_time();
#line 187 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_soilMoisture();
#line 198 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_irrigatingPressure();
#line 211 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_irrigatingVolume();
#line 222 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_scheduleDetailsA();
#line 233 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_scheduleDetailsB();
#line 244 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_scheduleDetailsC();
#line 255 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_scheduleDetailsD();
#line 266 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_scheduleDetailsE();
#line 277 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_SDError();
#line 289 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_settings_set( int _pos, int _year, int _month, int _day, int _hour, int _minute, int _startHour, int _startMin, int _durL, int _durH, int _seedlingInterval, int _seedlingTime);
#line 323 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
void display_screen_settings(bool _flash);
#line 13 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\EventManager.ino"
void event_update();
#line 109 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\EventManager.ino"
void event_log(String event, String value, bool display);
#line 119 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\EventManager.ino"
void event_logSerial(String event, String value);
#line 11 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
void flow_setup(int _interruptPin, float _volumePerTick);
#line 21 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
float flow_getFlowRate();
#line 26 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
float flow_getVolume();
#line 31 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
void flow_resetVolume();
#line 36 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
float flow_calcFlowRate();
#line 41 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
void flow_callback();
#line 50 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\IOManager.ino"
void io_setup();
#line 84 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\IOManager.ino"
void io_input_update();
#line 101 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\IOManager.ino"
void io_output_update();
#line 113 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\IOManager.ino"
void io_buttons_update();
#line 161 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\IOManager.ino"
void io_heartbeatLED_update();
#line 9 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
void sd_setup();
#line 65 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
void sd_error(bool _error);
#line 89 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
void sd_readSettings();
#line 270 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
void sd_writeSettings();
#line 329 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
void sd_log(String event, String value);
#line 374 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
void sd_checkSDCard();
#line 16 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SettingsEditor.ino"
void settings_next();
#line 47 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SettingsEditor.ino"
void settings_adjust();
#line 137 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SettingsEditor.ino"
void settings_update();
#line 146 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SettingsEditor.ino"
void settings_save();
#line 160 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SettingsEditor.ino"
void settings_load();
#line 81 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Main.ino"
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




#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\ClockManager.ino"
//settings:
SimpleThread clockResetTimer(86400000);			//clock reset interval (24hrs in ms)


//DS3231 Clock;
RTC_DS3231 Clock;
//uint32_t initTimeSeconds;

//bool reset = false;

void clock_setup()
{
	//reset = false;

	Wire.begin();
	
	//Serial.println("Wire started");
	
	if (! Clock.begin()) 
	{
		Serial.println("Couldn't find RTC Module");
		while (1);
	}
	
	if (Clock.lostPower()) 
	{
		Serial.println("RTC lost power, lets set the time!");
		Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
	
	//Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));			//uncomment to set time... Keep commented thereafter...
	
	clock_update();
	time_logErrorSince = time_now;
}

void clock_update()
{
	time_now = Clock.now();
}

void clock_set(int year, int month, int day, int hour, int minute)
{
	//Clock.adjust();
}
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\Control.ino"
SimpleThread timer_sequence = SimpleThread(1000);
SimpleThread timer_minute = SimpleThread(1000);

SimpleThread timer_seedlingInterval = SimpleThread(3600000);
SimpleThread timer_seedlingTime = SimpleThread(600000);



void control_setup()
{
	control_resetValveStates();
}

void control_update()
{
	if (state_manualPower)
	{
		state_sequencePos = state_manualZone;
		
		control_resetValveStates();
		control_resetSeedlingValve();
		state_valves[state_sequencePos] = true;
	}
	else
	{
		if (control_checkTime())
		{
			//sequence start time
			
			if (state_sequencePos == -1)
			{
				//sequence has not yet started:
				//alternate daily schedule
				//reset timers
				//start schedule
				
				Serial.println("//start schedule");
				
				state_heavyDay = !state_heavyDay;
				
				if (state_heavyDay) 	timer_sequence = SimpleThread(setting_scheduleMinutesPerZoneHeavy * 60000);
				else 					timer_sequence = SimpleThread(setting_scheduleMinutesPerZoneLight * 60000);
				timer_sequence.reset();
				
				state_sequencePos = 0;
				
				//return;
			}
		}
	
		if (state_sequencePos != -1)
		{
			//sequence is running:
			
			if (timer_sequence.check())
			{
				//timer has elapsed in the current sequence position
				//increment position
				//set valve outputs
				
				state_sequencePos ++;
				if (state_sequencePos >= 7)
				{
					//sequence is complete
					//reset sequence
					
					state_sequencePos = -1;
					control_resetValveStates();
				}
				else
				{
					//sequence is not complete
					//set correct output
					
					control_resetValveStates();
					state_valves[state_sequencePos] = true;
				}
			}
		}
		else 
		{
			//(state_sequencePos == -1
			//sequence has stopped:
			//reset valves
			
			control_resetValveStates();
		}

		if (!state_seedlingWatering)
		{
			control_resetSeedlingValve();

			if (timer_seedlingInterval.check())
			{
				state_valves[7] = true;
				timer_seedlingTime.reset();
				state_seedlingWatering = true;
			}
		}
		else
		{
			if (timer_seedlingTime.check())
			{
				control_resetSeedlingValve();
				state_seedlingWatering = false;
			}
		}
		
	}
	
	
	
}

bool control_checkTime()
{
	if (time_now.hour() == setting_scheduleStartHour && time_now.minute() == setting_scheduleStartMinute)
	{
		return true;
	}
	else if (time_now.hour() == (setting_scheduleStartHour + 6) && time_now.minute() == setting_scheduleStartMinute)
	{
		return true;
	}
	return false;
}

void control_resetValveStates()
{
	for (int i = 0; i < 7; i++)
	{
		state_valves[i] = false;
	}
}

void control_resetSeedlingValve()
{
	state_valves[7] = false;
}

void control_manualZoneIncrement()
{
	state_manualZone++;
	if (state_manualZone >= 8)
	{
		state_manualZone = 0;
	}
}

void control_manualPowerToggle()
{
	state_manualPower = !state_manualPower;
	if (!state_manualPower)
	{
		state_sequencePos = -1;
	}
}
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\DisplayManager.ino"
//settings:
SimpleThread timer_displayCycle(2500);					//display cycle time

SimpleThread timer_displayIrrigationCycle(2000);		//display flash time

SimpleThread timer_displaySettingsFlash(500);

LiquidCrystal_I2C lcd(0x27,16,2);				//0x27 is default i2c address, 16,2 is lcd resolution: 16 characters, 2 rows

//0x27
//0x57
//0x68

int currentScreen = 0;
int irrigatingCyclePos = 0;

String display_event;
String display_value;

bool 	display_settings = false;
bool 	display_settings_flash = false;
int 	display_settings_pos = 0;
int 	display_settings_year = 0;
int 	display_settings_month = 0;
int 	display_settings_day = 0;
int 	display_settings_hour = 0;
int 	display_settings_minute = 0;
int 	display_settings_sequenceStartHour = 0;
int 	display_settings_sequenceStartMinute = 0;
int 	display_settings_sequenceDurationLight = 0;
int 	display_settings_sequenceDurationHeavy = 0;
int 	display_settings_seedlingInterval = 0;
int 	display_settings_seedlingTime = 0;


void display_setup()
{
	//lcd.begin(16, 2);
	//lcd.init();
	//lcd.backlight();
	
	lcd.init(); // initialize the lcd
	// Print a message to the LCD.
	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("8 Valve Schedule");
	lcd.setCursor(0,1);
	lcd.print("Controller");
}

void display_update()
{
	if (display_settings)
	{
		//display modes for adjusting settings
		
		if (timer_displaySettingsFlash.check())
		{
			display_screen_settings(display_settings_flash);
			display_settings_flash = !display_settings_flash;
		}
	}
	else if (state_sequencePos!= -1 && currentScreen != -1)
	{
		if (timer_displayIrrigationCycle.check())
		{
			if (irrigatingCyclePos == 0)
			{
				display_screen_irrigatingPressure();
				irrigatingCyclePos++;
			}
			else if (irrigatingCyclePos == 1)
			{
				display_screen_irrigatingVolume();
				irrigatingCyclePos = 0;
			}
		}
		currentScreen = 0;
	}
	else if (timer_displayCycle.check())
	{
		//display cycle			- scroll to next screen every few seconds
		display_cycle();
		irrigatingCyclePos = 0;
	}
}

void display_cycle()
{
	//Serial.println(currentScreen);
	
	//currentScreen = 5;

	if (error_SD)
	{
		currentScreen = 7;
	}
	else if (currentScreen == 7)
	{
		currentScreen = 0;
	}
	
	
	switch (currentScreen)
	{
		case -1:
			//event screen
			currentScreen++;
		break;
		
		case 0:
			display_screen_time();
			currentScreen++;
		break;
		
		case 1:
			display_screen_scheduleDetailsA();
			currentScreen ++;
		break;
		
		case 2:
			display_screen_scheduleDetailsB();
			currentScreen ++;
		break;
		
		case 3:
			display_screen_scheduleDetailsC();
			currentScreen ++;
		break;

		case 4:
			display_screen_scheduleDetailsD();
			currentScreen ++;
		break;

		case 5:
			display_screen_scheduleDetailsE();
			currentScreen ++;
		break;
		
		case 6:
			display_screen_soilMoisture();
			currentScreen ++;
		break;
		
		case 7:
			display_screen_SDError();
		break;
	}
	
	
}

void display_screen_event(String event, String value)
{
	display_event = event;
	display_value = value;
	
	timer_displayCycle.reset();
	currentScreen = -1;
	
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(event);
	lcd.setCursor(0, 1);
	lcd.print(value);
	
	//delay(3000);
}

void display_screen_time()
{
	lcd.clear();
	char tim[16];
	//sprintf(tim, "Time %02d:%02d:%02d", time_now.hour(), time_now.minute(), time_now.second());
	sprintf(tim, "Time %02d:%02d", time_now.hour(), time_now.minute());
	//sprintf(tim, "Time %02X:%02X:%02X", Clock.getHour(h12, PM), Clock.getMinute(), Clock.getSecond());
	char dat[16];
	sprintf(dat, "Date %04d/%02d/%02d", time_now.year(), time_now.month(), time_now.day());
	//sprintf(dat, "Date 20%02X/%02X/%02X", Clock.getYear(), Clock.getMonth(Century), Clock.getDate());
	lcd.setCursor(0, 0);
	lcd.print(tim);
	lcd.setCursor(0, 1);
	lcd.print(dat);
}

void display_screen_soilMoisture()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Soil moisture:");
	lcd.setCursor(0, 1);
	char arr[16];
	sprintf(arr, "%d%%", state_soilMoisture);
	lcd.print(arr);
}

void display_screen_irrigatingPressure()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	char arr[16];
	sprintf(arr, "Zone %d", state_sequencePos + 1);
	lcd.print(arr);
	lcd.setCursor(0, 1);
	sprintf(arr, "%.2f bar", state_pressure);
	//sprintf(arr, "%.0f raw", state_pressure);
	lcd.print(arr);
}

void display_screen_irrigatingVolume()
{
	lcd.clear();
	char arr[16];
	sprintf(arr, "Zone %d", state_sequencePos + 1);
	lcd.print(arr);
	lcd.setCursor(0, 1);
	sprintf(arr, "%.0f litres", min(state_flowVolume, 999999));
	lcd.print(arr);
}

void display_screen_scheduleDetailsA()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Schedule:");
	lcd.setCursor(0, 1);
	char arr[16];
	sprintf(arr, "Start %02d:%02d", setting_scheduleStartHour, setting_scheduleStartMinute);
	lcd.print(arr);
}

void display_screen_scheduleDetailsB()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Schedule light:");
	lcd.setCursor(0, 1);
	char arr[16];
	sprintf(arr, "%d min per zone", setting_scheduleMinutesPerZoneLight);
	lcd.print(arr);
}

void display_screen_scheduleDetailsC()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Schedule heavy:");
	lcd.setCursor(0, 1);
	char arr[16];
	sprintf(arr, "%d min per zone", setting_scheduleMinutesPerZoneHeavy);
	lcd.print(arr);
}

void display_screen_scheduleDetailsD()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Seedling Time:");
	lcd.setCursor(0, 1);
	char arr[16];
	sprintf(arr, "Between %d min", setting_seedlingInterval);
	lcd.print(arr);
}

void display_screen_scheduleDetailsE()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Seedling Time:");
	lcd.setCursor(0, 1);
	char arr[16];
	sprintf(arr, "Duration %d min", setting_seedlingTime);
	lcd.print(arr);
}

void display_screen_SDError()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("SD Card Error:");
	lcd.setCursor(0, 1);
	TimeSpan dur = time_now - time_logErrorSince;
	char since_buf[16];
	sprintf(since_buf, "For %03dd %02dh%02dm", dur.days(), dur.hours(), dur.minutes());
	lcd.print(since_buf);
}

void display_settings_set(	int _pos, 
							int _year, 
							int _month, 
							int _day, 
							int _hour, 
							int _minute, 
							int _startHour, 
							int _startMin, 
							int _durL, 
							int _durH,
							int _seedlingInterval,
							int _seedlingTime)
{
	display_settings_pos = _pos;
	display_settings_year = _year;
	display_settings_month = _month;
	display_settings_day = _day;
	display_settings_hour = _hour;
	display_settings_minute = _minute;
	display_settings_sequenceStartHour = _startHour;
	display_settings_sequenceStartMinute = _startMin;
	display_settings_sequenceDurationLight = _durL;
	display_settings_sequenceDurationHeavy = _durH;
	display_settings_seedlingInterval = _seedlingInterval;
	display_settings_seedlingTime = _seedlingTime;
	
	display_settings = true;
	if (_pos == -1) 
	{
		display_settings = false;
		currentScreen = 0;
	}
}

void display_screen_settings(bool _flash)
{
	
	lcd.clear();
	char bufA[16];
	char bufB[16];
	
	if (display_settings_pos == 0)		// YEAR
	{
		if (_flash)
		{
			sprintf(bufA, "Setting Date:");
			sprintf(bufB, "%04d/%02d/%02d", display_settings_year, display_settings_month, display_settings_day);
		}
		else
		{
			sprintf(bufA, "Setting Date:");
			sprintf(bufB, "    /%02d/%02d", display_settings_month, display_settings_day);
		}
	}
	else if (display_settings_pos == 1)	// MONTH
	{
		if (_flash)
		{
			sprintf(bufA, "Setting Date:");
			sprintf(bufB, "%04d/%02d/%02d", display_settings_year, display_settings_month, display_settings_day);
		}
		else
		{
			sprintf(bufA, "Setting Date:");
			sprintf(bufB, "%04d/  /%02d", display_settings_year, display_settings_day);
		}
	}
	else if (display_settings_pos == 2)	// DAY
	{
		if (_flash)
		{
			sprintf(bufA, "Setting Date:");
			sprintf(bufB, "%04d/%02d/%02d", display_settings_year, display_settings_month, display_settings_day);
		}
		else
		{
			sprintf(bufA, "Setting Date:");
			sprintf(bufB, "%04d/%02d/  ", display_settings_year, display_settings_month);
		}
	}
	else if (display_settings_pos == 3)	// HOUR
	{
		if (_flash)
		{
			sprintf(bufA, "Setting Time:");
			sprintf(bufB, "%02d:%02d", display_settings_hour, display_settings_minute);
		}
		else
		{
			sprintf(bufA, "Setting Time:");
			sprintf(bufB, "  :%02d", display_settings_minute);
		}
	}
	else if (display_settings_pos == 4)	// MINUTE
	{
		if (_flash)
		{
			sprintf(bufA, "Setting Time:");
			sprintf(bufB, "%02d:%02d", display_settings_hour, display_settings_minute);
		}
		else
		{
			sprintf(bufA, "Setting Time:");
			sprintf(bufB, "%02d:  ", display_settings_hour);
		}
	}
	else if (display_settings_pos == 5)	// SEQUENCE START HOUR
	{
		if (_flash)
		{
			sprintf(bufA, "Schedule Start:");
			sprintf(bufB, "%02d:%02d", display_settings_sequenceStartHour, display_settings_sequenceStartMinute);
		}
		else
		{
			sprintf(bufA, "Schedule Start:");
			sprintf(bufB, "  :%02d", display_settings_sequenceStartMinute);
		}
	}
	else if (display_settings_pos == 6)	// SEQUENCE START MINUTE
	{
		if (_flash)
		{
			sprintf(bufA, "Schedule Start:");
			sprintf(bufB, "%02d:%02d", display_settings_sequenceStartHour, display_settings_sequenceStartMinute);
		}
		else
		{
			sprintf(bufA, "Schedule Start:");
			sprintf(bufB, "%02d:  ", display_settings_sequenceStartHour);
		}
	}
	else if (display_settings_pos == 7)	// SEQUENCE DURATION LIGHT
	{
		if (_flash)
		{
			sprintf(bufA, "Time per zone:");
			sprintf(bufB, "Light %02d mins", display_settings_sequenceDurationLight);
		}
		else
		{
			sprintf(bufA, "Time per zone:");
			sprintf(bufB, "Light    mins");
		}
	}
	else if (display_settings_pos == 8)	// SEQUENCE DURATION HEAVY
	{
		if (_flash)
		{
			sprintf(bufA, "Time per zone:");
			sprintf(bufB, "Heavy %02d mins", display_settings_sequenceDurationHeavy);
		}
		else
		{
			sprintf(bufA, "Time per zone:");
			sprintf(bufB, "Heavy    mins");
		}
	}
	else if (display_settings_pos == 9)	// SEEDLING INTERVAL
	{
		if (_flash)
		{
			sprintf(bufA, "Seedling Time:");
			sprintf(bufB, "Between %02d min", display_settings_seedlingInterval);
		}
		else
		{
			sprintf(bufA, "Seedling Time:");
			sprintf(bufB, "Between    min");
		}
	}
	else if (display_settings_pos == 10)	// SEEDLING TIME
	{
		if (_flash)
		{
			sprintf(bufA, "Seedling Time:");
			sprintf(bufB, "Duration %02d min", display_settings_seedlingTime);
		}
		else
		{
			sprintf(bufA, "Seedling Time:");
			sprintf(bufB, "Duration    min");
		}
	}
	
	
	lcd.setCursor(0, 0);
	lcd.print(bufA);
	lcd.setCursor(0, 1);
	lcd.print(bufB);
}
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\EventManager.ino"
SimpleThread timer_flowLog(120000);
SimpleThread timer_sensorLog(60000);

//event flags
bool flag_devicePower = false;
int flag_sequencePos = -1;
bool flag_heavyDay = true;

bool flag_manualPower = false;
int flag_manualZone = 0;
bool flag_errorSD = false;

void event_update()
{
	//detect differences between flags and states
	if (!flag_devicePower)
	{
							event_log("POWER", "ON", true);
		char arr[16];
		sprintf(arr, "%02d:%02d", setting_scheduleStartHour, setting_scheduleStartMinute);
							event_log("SCHEDULE_START", arr, false);
		sprintf(arr, "%02d:%02d", setting_scheduleMinutesPerZoneLight/60, setting_scheduleMinutesPerZoneLight%60);
							event_log("SCHEDULE_TIME_PER_ZONE_LIGHT", arr, false);
		sprintf(arr, "%02d:%02d", setting_scheduleMinutesPerZoneHeavy/60, setting_scheduleMinutesPerZoneHeavy%60);
							event_log("SCHEDULE_TIME_PER_ZONE_HEAVY", arr, false);
		sprintf(arr, "%.2f", setting_flowVolumePerTick);
							event_log("FLOW_VOLUME_PER_TICK", arr, false);
	}
	
						
						
	if (state_sequencePos != flag_sequencePos)
	{
		if (flag_sequencePos != -1)
		{
			float volume = flow_getVolume();
			char arr[16];
			sprintf(arr, "%.0f", min(volume, 999999));
			char arr2[16];
			sprintf(arr2, "LITRES_ZONE_%02d", flag_sequencePos+1);
							event_log(arr2, arr, false);
			flow_resetVolume();
		}
		switch (state_sequencePos)
		{
			case (-1):		event_log("IRRIGATION", "STOPPED", 	false); 	break;
			case (0):		event_log("ZONE", "1", 				true); 	break;
			case (1):		event_log("ZONE", "2", 				true); 	break;
			case (2):		event_log("ZONE", "3", 				true); 	break;
			case (3):		event_log("ZONE", "4", 				true); 	break;
			case (4):		event_log("ZONE", "5", 				true); 	break;
			case (5):		event_log("ZONE", "6", 				true); 	break;
			case (6):		event_log("ZONE", "7", 				true); 	break;
			case (7):		event_log("ZONE", "8", 				true); 	break;
		}
	}
	
	if (state_manualPower && !flag_manualPower)
							event_log("MANUAL_MODE", "ENABLED", true);
	if (!state_manualPower && flag_manualPower)
							event_log("AUTOMATIC_MODE", "ENABLED", true);
	
	
	
	if (state_heavyDay && !flag_heavyDay)
	{
							event_log("DAY", "HEAVY_WATERING", true);
	}
	else if (!state_heavyDay && flag_heavyDay)
	{
							event_log("DAY", "LIGHT_WATERING", true);
	}
	if (!error_SD && flag_errorSD)
	{
		TimeSpan dur = time_now - time_logErrorSince;
		char since_buf[16];
		sprintf(since_buf, "Duration: %03dd %02dh%02dm", dur.days(), dur.hours(), dur.minutes());
							event_log("SD_ERROR_RESOLVED", since_buf, true);
	}
	
	if (time_now.minute() == 0)
	//if (true)
	{
		if (timer_flowLog.check())
		{
			
		}
	}
	
	if (timer_sensorLog.check())
	{
		char arr[16];
		sprintf(arr, "%d", state_soilMoisture);
							event_log("SOIL_MOISTURE(%)", arr, false);
		sprintf(arr, "%.2f", state_pressure);
							event_log("WATER_PRESSURE(bar)", arr, false);
	}
	
	//update flags
	flag_devicePower 			= true;
	flag_sequencePos			= state_sequencePos;
	flag_heavyDay				= state_heavyDay;
	flag_manualPower			= state_manualPower;
	flag_manualZone				= state_manualZone;
	flag_errorSD				= error_SD;
	
}

void event_log(String event, String value, bool display)
{
	if (display)
	{
		display_screen_event(event, value);
	}
	event_logSerial(event, value);
	sd_log(event, value);
}

void event_logSerial(String event, String value)
{
	char dat[30];
	char tim[30];
	sprintf(dat, "%04d/%02d/%02d", time_now.year(), time_now.month(), time_now.day());
	sprintf(tim, "%02d:%02d:%02d", time_now.hour(), time_now.minute(), time_now.second());
	
	Serial.print("logging event: ");
	Serial.print(dat);
	Serial.print(" - ");
	Serial.print(tim);
	Serial.print(" - ");
	Serial.print(event);
	Serial.print(" - ");
	Serial.println(value);
}

#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\FlowMeter.ino"
const unsigned long debounceTime = 300;		//ms
float volumePerTick;
float currentFlowRate = 0;
float currentVolume;
int flowPin;
bool flowSensor;
bool flowSensorPrev;
unsigned long lastTick;


void flow_setup(int _interruptPin, float _volumePerTick)
{
	flowPin = _interruptPin;
	volumePerTick = _volumePerTick;
	pinMode(_interruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(_interruptPin), flow_callback, CHANGE);
	lastTick = millis();
}


float flow_getFlowRate()
{
	return min(currentFlowRate, flow_calcFlowRate());
}

float flow_getVolume()
{
	return currentVolume;
}

void flow_resetVolume()
{
	currentVolume = 0;
}

float flow_calcFlowRate()
{
	return (3600000 / ((millis() - lastTick))) * volumePerTick;
}

void flow_callback()
{
	flowSensor = digitalRead(flowPin);
	if (millis() - lastTick > debounceTime)
	{
		//sufficient time has passed:
		
		if (!flowSensor && flowSensorPrev)
		{
			//input is different to last logged value:
			//currentFlowRate = flow_calcFlowRate();
			currentVolume += volumePerTick;
			lastTick = millis();
			Serial.println("pulse");
		}
	}
	flowSensorPrev = flowSensor;
}
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\IOManager.ino"
//mcp object
Adafruit_MCP23017 mcp;

//MCP outputs
const int mcpPin_valve0 = 0;
const int mcpPin_valve1 = 1;
const int mcpPin_valve2 = 2;
const int mcpPin_valve3 = 3;
const int mcpPin_valve4 = 4;
const int mcpPin_valve5 = 5;
const int mcpPin_valve6 = 6;
const int mcpPin_valve7 = 7;		//seedling zone

//MCP inputs
const int mcpPin_buttonA = 10;
const int mcpPin_buttonB = 11;
const int mcpPin_buttonC = 8;
const int mcpPin_buttonD = 9;

//M0 inputs
const int pin_moisture = A1;
const int pin_flow = 7;
const int pin_pressure = A0;

//Analog Calibration
float input_moistureMin = 410;
float input_moistureMax = 802;
float input_pressureCalLo = 109;				// raw digital value with zero added pressure
float input_pressureCalHi = 325;				// raw digital value with [input_pressureCalHiPressure] amount of added pressure
float input_pressureCalHiPressure = 2.45;		//bar value of input_pressureCalHi

//DotStar LED
Adafruit_DotStar px(1, 41, 40, DOTSTAR_BRG);
SimpleThread timer_heartbeatLED = SimpleThread(25);
int heartbeatLED_brightness = 0;
int heartbeatLED_increment = 5;
bool state_heartbeatLEDDirection = false;

//Buttons
bool state_buttonA_prev = false;
bool state_buttonB_prev = false;
bool state_buttonC_prev = false;
bool state_buttonD_prev = false;
const unsigned long debounceTimeButton = 100;		//ms
unsigned long lastPressedA;
unsigned long lastPressedB;
unsigned long lastPressedC;
unsigned long lastPressedD;

void io_setup()
{
	//set input pins
	pinMode(pin_moisture, INPUT_PULLUP);
	pinMode(pin_flow, INPUT_PULLUP);
	pinMode(pin_pressure, INPUT_PULLUP);
	
	mcp.begin(); // use default address 0
	mcp.pinMode(mcpPin_valve0, OUTPUT);
	mcp.pinMode(mcpPin_valve1, OUTPUT);
	mcp.pinMode(mcpPin_valve2, OUTPUT);
	mcp.pinMode(mcpPin_valve3, OUTPUT);
	mcp.pinMode(mcpPin_valve4, OUTPUT);
	mcp.pinMode(mcpPin_valve5, OUTPUT);
	mcp.pinMode(mcpPin_valve6, OUTPUT);
	mcp.pinMode(mcpPin_valve7, OUTPUT);
	mcp.pinMode(mcpPin_buttonA, INPUT);
	mcp.pinMode(mcpPin_buttonB, INPUT);
	mcp.pinMode(mcpPin_buttonC, INPUT);
	mcp.pinMode(mcpPin_buttonD, INPUT);
	mcp.pullUp(mcpPin_buttonA, HIGH); // turn on a 100K pullup internally
	mcp.pullUp(mcpPin_buttonB, HIGH);
	mcp.pullUp(mcpPin_buttonC, HIGH);
	mcp.pullUp(mcpPin_buttonD, HIGH);
	Serial.println("setup: mcp sucess");

	// Flow sensor stuff:
	flow_setup(pin_flow, setting_flowVolumePerTick);
	
	//DotStar
	px.begin(); // Initialize pins for output
	px.show();  // Turn all LEDs off ASAP
}

void io_input_update()
{
	uint16_t input_soilMoisture = analogRead(pin_moisture);
	uint16_t input_pressure = analogRead(pin_pressure);
	
	state_soilMoisture = (int) 100 * (1 - ((input_soilMoisture - input_moistureMin) / (input_moistureMax - input_moistureMin)));
	state_soilMoisture = min(max(state_soilMoisture, 0), 100);
	
	state_pressure = max(input_pressure - input_pressureCalLo, 0) * (input_pressureCalHiPressure/(input_pressureCalHi - input_pressureCalLo));
	state_pressure = min(max(state_pressure, 0), 16);
	
	//state_flowRate = flow_getFlowRate();
	state_flowVolume = flow_getVolume();

	io_buttons_update();
}

void io_output_update()
{
	mcp.digitalWrite(mcpPin_valve0, state_valves[0]);
	mcp.digitalWrite(mcpPin_valve1, state_valves[1]);
	mcp.digitalWrite(mcpPin_valve2, state_valves[2]);
	mcp.digitalWrite(mcpPin_valve3, state_valves[3]);
	mcp.digitalWrite(mcpPin_valve4, state_valves[4]);
	mcp.digitalWrite(mcpPin_valve5, state_valves[5]);
	mcp.digitalWrite(mcpPin_valve6, state_valves[6]);
	mcp.digitalWrite(mcpPin_valve7, state_valves[7]);
}

void io_buttons_update()
{
	state_buttonA = !mcp.digitalRead(mcpPin_buttonA);
	state_buttonB = !mcp.digitalRead(mcpPin_buttonB);
	state_buttonC = !mcp.digitalRead(mcpPin_buttonC);
	state_buttonD = !mcp.digitalRead(mcpPin_buttonD);
	
	if (millis() - lastPressedA > debounceTimeButton)
	{
		if (state_buttonA && !state_buttonA_prev)
		{
			lastPressedA = millis();
			settings_next();
		}
	}
	state_buttonA_prev = state_buttonA;
	
	if (millis() - lastPressedB > debounceTimeButton)
	{
		if (state_buttonB && !state_buttonB_prev)
		{
			lastPressedB = millis();
			settings_adjust();
		}
	}
	state_buttonB_prev = state_buttonB;
	
	if (millis() - lastPressedC > debounceTimeButton)
	{
		if (state_buttonC && !state_buttonC_prev)
		{
			lastPressedC = millis();
			control_manualPowerToggle();
		}
	}
	state_buttonC_prev = state_buttonC;

	if (millis() - lastPressedD > debounceTimeButton)
	{
		if (state_buttonD && !state_buttonD_prev)
		{
			lastPressedD = millis();
			control_manualZoneIncrement();
		}
	}
	state_buttonD_prev = state_buttonD;
}

void io_heartbeatLED_update()
{
	if (timer_heartbeatLED.check())
	{
		////index, G, R, B
		//if (state_heartbeatLED)
		//{
		//	px.setPixelColor(0, 150, 0, 0); // Set the pixel colors
		//	px.show();              // Refresh strip
		//	state_heartbeatLED = false;
		//}
		//else
		//{
		//	px.setPixelColor(0, 0, 0, 0); // Set the pixel colors
		//	px.show();              // Refresh strip
		//	state_heartbeatLED = true;
		//}

		if (state_heartbeatLEDDirection)
		{
			//UP:
			heartbeatLED_brightness += heartbeatLED_increment;

			if (heartbeatLED_brightness >= 255)
			{
				heartbeatLED_brightness = 255;
				state_heartbeatLEDDirection = false;
			}
		}
		else
		{
			//down:
			heartbeatLED_brightness -= heartbeatLED_increment;

			if (heartbeatLED_brightness <= 0)
			{
				heartbeatLED_brightness = 0;
				state_heartbeatLEDDirection = true;
			}
		}

		int r = 0;
		int g = 0;
		int b = 0;

		if (error_SD)
		{
			r = heartbeatLED_brightness;
			g = 0;
		}
		else
		{
			r = 0;
			g = heartbeatLED_brightness;
		}
		if (state_sequencePos != -1 || state_seedlingWatering)
		{
			b = 255 - heartbeatLED_brightness;
		}
		else
		{
			b = 0;
		}
		//index, G, R, B
		px.setPixelColor(0, g, r, b); // Set the pixel colors
		px.show();              // Refresh strip
	}
}
#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SDCard.ino"
//sd card logging
File file_log;
File file_settings;
const String fileName_log = "log.csv";
//const String fileName_settings = "settings.ini";
const char *fileName_settings = "settings.ini";
const int pin_cs = A5;

void sd_setup()
{
	pinMode(pin_cs, OUTPUT);

	//file_log.close();
	//file_settings.close();

	// SD Card Initialization
	if (SD.begin(pin_cs))
	{
		Serial.println("SD card connected");
		sd_error(false);
	}
	else
	{
		Serial.println("SD card connection failed");
		sd_error(true);
		return;
	}

	if (!SD.exists(fileName_log))
	{
		Serial.println("Log file does not exist");
		// Create/Open file
		file_log = SD.open(fileName_log, FILE_WRITE);
		if (file_log)
		{
			Serial.println("Creating log file");
			file_log.println("Date,Time,Event,Value");  // Write to file
			file_log.close();
		}
	}
	else
	{
		Serial.println("Existing log file found");
	}
	/*
	if (!SD.exists(fileName_settings))
	{
		Serial.println("Settings file does not exist");
		// Create/Open file
		file_settings = SD.open(fileName_settings, FILE_WRITE);
		if (file_settings)
		{
			Serial.println("Creating settings file");
			//file_log.println("Date,Time,Event,Value");  // Write to file
			file_log.close();
		}
	}
	else
	{
		Serial.println("Existing settings file found");
	}
	*/
}

void sd_error(bool _error)
{
	if (_error)
	{
		if (!error_SD)
		{
			time_logErrorSince = time_now;
			timerSDCheck.setInterval(sdCheck_error);
		}
		error_SD = true;
		Serial.println("error_SD = true");
	}
	else
	{
		if (error_SD)
		{
			timerSDCheck.setInterval(sdCheck_good);
		}
		error_SD = false;
		Serial.println("error_SD = false");
	}
	
}

void sd_readSettings()
{
	/*
	int setting_scheduleStartHour = 8;
	int setting_scheduleStartMinute = 30;
	int setting_scheduleMinutesPerZone = 10;
	float setting_flowVolumePerTick = 0.5;
	*/
	
	const size_t bufferLen = 80;
	char buffer[bufferLen];

	IniFile ini(fileName_settings);
	if (!ini.open())
	{
		Serial.print("Ini file ");
		Serial.print(fileName_settings);
		Serial.println(" does not exist");
		sd_error(true);
		return;
	}
	Serial.println("Ini file exists");

	// Check the file is valid. This can be used to warn if any lines
	// are longer than the buffer.
	if (!ini.validate(buffer, bufferLen))
	{
		Serial.print("Ini file ");
		Serial.print(fileName_settings);
		Serial.println(" not valid -> error_SD = true");
		//printErrorMessage(ini.getError());
		sd_error(true);
		return;
	}
	else
	{
		Serial.println("Ini file valid -> error_SD = false");
		sd_error(false);
	}
	
	
	
	// Fetch a value from a key which is present
	if (ini.getValue("SCHEDULE", "start_time", buffer, bufferLen))
	{
		//Serial.print("section 'SCHEDULE' has an entry 'start_time' with value ");
		//Serial.println(buffer);
		
		char buf[2];
		buf[0] = buffer[0];
		buf[1] = buffer[1];
		setting_scheduleStartHour = atoi(buf);
		buf[0] = buffer[3];
		buf[1] = buffer[4];
		setting_scheduleStartMinute = atoi(buf);
		Serial.print("Schedule start: ");
		Serial.print(setting_scheduleStartHour);
		Serial.print(":");
		Serial.println(setting_scheduleStartMinute);
	}
	else
	{
		Serial.print("Could not read 'start_time' from section 'SCHEDULE'");
		sd_error(true);
		return;
	}
	
	// Fetch a value from a key which is present
	if (ini.getValue("SCHEDULE", "time_per_zone_light", buffer, bufferLen))
	{
		//Serial.print("section 'SCHEDULE' has an entry 'time_per_zone_light' with value ");
		//Serial.println(buffer);
		
		char buf[2];
		buf[0] = buffer[0];
		buf[1] = buffer[1];
		int hours = atoi(buf);
		buf[0] = buffer[3];
		buf[1] = buffer[4];
		setting_scheduleMinutesPerZoneLight = 60*hours + atoi(buf);
		Serial.print("Schedule mins per zone light: ");
		Serial.println(setting_scheduleMinutesPerZoneLight);
	}
	else
	{
		Serial.print("Could not read 'time_per_zone_light' from section 'SCHEDULE'");
		sd_error(true);
		return;
	}
	
	// Fetch a value from a key which is present
	if (ini.getValue("SCHEDULE", "time_per_zone_heavy", buffer, bufferLen))
	{
		//Serial.print("section 'SCHEDULE' has an entry 'time_per_zone_heavy' with value ");
		//Serial.println(buffer);
		
		char buf[2];
		buf[0] = buffer[0];
		buf[1] = buffer[1];
		int hours = atoi(buf);
		buf[0] = buffer[3];
		buf[1] = buffer[4];
		setting_scheduleMinutesPerZoneHeavy = 60*hours + atoi(buf);
		Serial.print("Schedule mins per zone heavy: ");
		Serial.println(setting_scheduleMinutesPerZoneHeavy);
	}
	else
	{
		Serial.print("Could not read 'time_per_zone_heavy' from section 'SCHEDULE'");
		sd_error(true);
		return;
	}

	if (ini.getValue("SCHEDULE", "seedling_interval", buffer, bufferLen))
	{
		//Serial.print("section 'SCHEDULE' has an entry 'seedling_interval' with value ");
		//Serial.println(buffer);
		
		char buf[2];
		buf[0] = buffer[0];
		buf[1] = buffer[1];
		int hours = atoi(buf);
		buf[0] = buffer[3];
		buf[1] = buffer[4];
		setting_seedlingInterval = 60*hours + atoi(buf);
		Serial.print("Schedule seedling_interval: ");
		Serial.println(setting_seedlingInterval);
	}
	else
	{
		Serial.print("Could not read 'seedling_interval' from section 'SCHEDULE'");
		sd_error(true);
		return;
	}

	if (ini.getValue("SCHEDULE", "seedling_time", buffer, bufferLen))
	{
		//Serial.print("section 'SCHEDULE' has an entry 'seedling_time' with value ");
		//Serial.println(buffer);
		
		char buf[2];
		buf[0] = buffer[0];
		buf[1] = buffer[1];
		int hours = atoi(buf);
		buf[0] = buffer[3];
		buf[1] = buffer[4];
		setting_seedlingTime = 60*hours + atoi(buf);
		Serial.print("Schedule seedling_time: ");
		Serial.println(setting_seedlingTime);
	}
	else
	{
		Serial.print("Could not read 'seedling_time' from section 'SCHEDULE'");
		sd_error(true);
		return;
	}
	
	// Fetch a value from a key which is present
	if (ini.getValue("FLOW_METER", "volume_per_tick", buffer, bufferLen))
	{
		//Serial.print("section 'FLOW_METER' has an entry 'volume_per_tick' with value ");
		//Serial.println(buffer);
		
		//char buf[2];
		//buf[0] = buffer[0];
		//buf[1] = buffer[1];
		//int hours = atoi(buf);
		//buf[0] = buffer[3];
		//buf[1] = buffer[4];
		setting_flowVolumePerTick = atof(buffer);
		Serial.print("FLOW_METER volume per tick: ");
		Serial.println(setting_flowVolumePerTick);
	}
	else
	{
		Serial.print("Could not read 'volume_per_tick' from section 'FLOW_METER'");
		sd_error(true);
		return;
	}
}

void sd_writeSettings()
{
	// delete the file:
	Serial.println("Removing settings");
	SD.remove(fileName_settings);
	
	Serial.print("Creating ");
	Serial.println(fileName_settings);
	
	file_settings = SD.open(fileName_settings, FILE_WRITE);
	if (file_settings)
	{
		file_settings.println("[SCHEDULE]");
		file_settings.println("# time format is hh:mm");
		
		char buf[16];
		sprintf(buf, "start_time = %02d:%02d", setting_scheduleStartHour, setting_scheduleStartMinute);
		file_settings.println(buf);
		
		sprintf(buf, "time_per_zone_light = %02d:%02d", setting_scheduleMinutesPerZoneLight/60, setting_scheduleMinutesPerZoneLight%60);
		file_settings.println(buf);
		
		sprintf(buf, "time_per_zone_heavy = %02d:%02d", setting_scheduleMinutesPerZoneHeavy/60, setting_scheduleMinutesPerZoneHeavy%60);
		file_settings.println(buf);

		sprintf(buf, "seedling_interval = %02d:%02d", setting_seedlingInterval/60, setting_seedlingInterval%60);
		file_settings.println(buf);

		sprintf(buf, "seedling_time = %02d:%02d", setting_seedlingTime/60, setting_seedlingTime%60);
		file_settings.println(buf);
		
		file_settings.println();
		
		file_settings.println("[FLOW_METER]");
		file_settings.println("# unit for volume_per_tick is litres");
		
		sprintf(buf, "volume_per_tick = %.2f", setting_flowVolumePerTick);
		file_settings.println(buf);
		
		file_settings.close();
	}
	
	
	
	
	/*
	[SCHEDULE]
	# time format is hh:mm
	start_time = 07:45
	time_per_zone = 01:10

	[FLOW_METER]
	# unit for volume_per_tick is litres
	volume_per_tick = 0.6

	*/
	
}

void sd_log(String event, String value)
{
	if (error_SD)
	{
		Serial.println("Attempting to reconnect sd card");
		sd_setup();
	}

	if (!error_SD)
	{
		SD.exists(fileName_log);

		char dat[30];
		char tim[30];
		sprintf(dat, "%04d/%02d/%02d", time_now.year(), time_now.month(), time_now.day());
		sprintf(tim, "%02d:%02d:%02d", time_now.hour(), time_now.minute(), time_now.second());

		// Create/Open file
		file_log = SD.open(fileName_log, FILE_WRITE);

		if (file_log)
		{
			// if the file opened okay, write to it:
			//Serial.println("Writing to file...");
			// Write to file
			file_log.print(dat);
			file_log.print(",");
			file_log.print(tim);
			file_log.print(",");
			file_log.print(event);
			file_log.print(",");
			file_log.println(value);
			file_log.close(); // close the file

			//time_logErrorSince = time_now;
		}
		// if the file didn't open, print an error:
		else
		{
			sd_error(true);
			Serial.println("Error writing to log");
		}
	}
}

void sd_checkSDCard()
{
	//check card
	sd_readSettings();
}

#line 1 "d:\\_work\\Waterworks Automation\\8 Valve Schedule Controller\\Arduino\\Main\\SettingsEditor.ino"
SimpleThread timer_settingsTimeout(20000);

bool setting_loaded = false;
int setting_pos = -1;
int setting_year; 
int setting_month;
int setting_day;
int setting_hour;
int setting_minute;
int setting_startHour;
int setting_startMin;
int setting_durL;
int setting_durH;
	
	
void settings_next()
{
	setting_pos++;
	
	if (setting_pos == 0 && !setting_loaded)
	{
		settings_load();
		setting_loaded = true;
	}
	else if (setting_pos >= 11)
	{
		setting_pos = -1;
		settings_save();
	}
	
	
	
	display_settings_set(setting_pos, 
							setting_year, 
							setting_month, 
							setting_day, 
							setting_hour, 
							setting_minute, 
							setting_startHour, 
							setting_startMin, 
							setting_durL, 
							setting_durH, 
							setting_seedlingInterval, 
							setting_seedlingTime);
}

void settings_adjust()
{
	if (setting_pos == 0)
	{
		setting_year++;
	}
	else if (setting_pos == 1)
	{
		setting_month++;
	}
	else if (setting_pos == 2)
	{
		setting_day++;
	}
	else if (setting_pos == 3)
	{
		setting_hour++;
	}
	else if (setting_pos == 4)
	{
		setting_minute++;
	}
	else if (setting_pos == 5)
	{
		setting_startHour++;
	}
	else if (setting_pos == 6)
	{
		setting_startMin++;
	}
	else if (setting_pos == 7)
	{
		setting_durL++;
	}
	else if (setting_pos == 8)
	{
		setting_durH++;
	}
	
	if (setting_year >= 2050)
	{
		setting_year = 2019;
	}
	if (setting_month >= 13)
	{
		setting_month = 1;
	}
	if (setting_day >= 32)
	{
		setting_day = 1;
	}
	if (setting_hour >= 24)
	{
		setting_hour = 0;
	}
	if (setting_minute >= 60)
	{
		setting_minute = 0;
	}
	if (setting_startHour >= 24)
	{
		setting_startHour = 0;
	}
	if (setting_startMin >= 60)
	{
		setting_startMin = 0;
	}
	if (setting_durL >= 59)
	{
		setting_durL = 1;
	}
	if (setting_durH >= 59)
	{
		setting_durH = 1;
	}
	
	display_settings_set(setting_pos, 
							setting_year, 
							setting_month, 
							setting_day, 
							setting_hour, 
							setting_minute, 
							setting_startHour, 
							setting_startMin, 
							setting_durL, 
							setting_durH, 
							setting_seedlingInterval, 
							setting_seedlingTime);
}

void settings_update()
{
	if (timer_settingsTimeout.check())
	{
		settings_save();
	}
	
}

void settings_save()
{
	display_settings_set(-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	DateTime adjusted(setting_year, setting_month, setting_day, setting_hour, setting_minute, 0);
	Clock.adjust(adjusted);
	
	setting_scheduleStartHour = setting_startHour;
	setting_scheduleStartMinute = setting_startMin;
	setting_scheduleMinutesPerZoneLight = setting_durL;
	setting_scheduleMinutesPerZoneHeavy = setting_durH;
	
	sd_writeSettings();
}

void settings_load()
{
	setting_year = time_now.year();
	setting_month = time_now.month();
	setting_day = time_now.day();
	setting_hour = time_now.hour();
	setting_minute = time_now.minute();
	setting_startHour = setting_scheduleStartHour;
	setting_startMin = setting_scheduleStartMinute;
	setting_durL = setting_scheduleMinutesPerZoneLight;
	setting_durH = setting_scheduleMinutesPerZoneHeavy;
}
