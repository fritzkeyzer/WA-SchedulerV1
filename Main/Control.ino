SimpleThread timer_sequence = SimpleThread(1000);
SimpleThread timer_minute = SimpleThread(1000);


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
	for (int i = 0; i < 8; i++)
	{
		state_valves[i] = false;
	}
}

void control_manualZoneIncrement()
{
	state_manualZone++;
	if (state_manualZone >= 7)
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