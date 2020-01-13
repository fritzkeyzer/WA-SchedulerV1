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
		sprintf(arr, "%02d:%02d", setting_seedlingInterval/60, setting_seedlingInterval%60);
							event_log("SCHEDULE_SEEDLING_INTERVAL", arr, false);
		sprintf(arr, "%02d:%02d", setting_seedlingTime/60, setting_seedlingTime%60);
							event_log("SCHEDULE_SEEDLING_TIME", arr, false);
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
							event_log("MODE", "MANUAL", true);
	if (!state_manualPower && flag_manualPower)
							event_log("MODE", "AUTOMATIC", true);
	
	
	
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
