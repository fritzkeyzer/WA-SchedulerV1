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