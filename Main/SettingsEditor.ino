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
	else if (setting_pos >= 9)
	{
		setting_pos = -1;
		settings_save();
	}
	
	
	
	display_settings_set(setting_pos, setting_year, setting_month, setting_day, setting_hour, setting_minute, setting_startHour, setting_startMin, setting_durL, setting_durH);
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
	
	display_settings_set(setting_pos, setting_year, setting_month, setting_day, setting_hour, setting_minute, setting_startHour, setting_startMin, setting_durL, setting_durH);
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
	display_settings_set(-1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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