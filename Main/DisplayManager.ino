//settings:
SimpleThread timer_displayCycle(4000);					//display cycle time

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
			display_screen_soilMoisture();
			if (error_logging)			currentScreen = 5;
			else if (error_settings)	currentScreen = 6;
			else						currentScreen = 0;
		break;
		
		case 5:
			display_screen_logError();
			if (error_settings)			currentScreen = 6;
			else						currentScreen = 0;
		break;
		
		case 6:
			display_screen_settingsError();
			currentScreen = 0;
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

void display_screen_logError()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Logging Error:");
	lcd.setCursor(0, 1);
	TimeSpan dur = time_now - time_logErrorSince;
	char since_buf[16];
	sprintf(since_buf, "For %03dd %02dh%02dm", dur.days(), dur.hours(), dur.minutes());
	lcd.print(since_buf);
}

void display_screen_settingsError()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Settings Error:");
	//lcd.setCursor(0, 1);
	//TimeSpan dur = time_now - time_logErrorSince;
	//char since_buf[16];
	//sprintf(since_buf, "For %03dd %02dh%02dm", dur.days(), dur.hours(), dur.minutes());
	//lcd.print(since_buf);
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
							int _durH)
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
	
	
	lcd.setCursor(0, 0);
	lcd.print(bufA);
	lcd.setCursor(0, 1);
	lcd.print(bufB);
}