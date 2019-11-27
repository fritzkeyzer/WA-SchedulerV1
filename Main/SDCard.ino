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
