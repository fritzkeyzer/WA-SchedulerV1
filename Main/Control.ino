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
		//state_sequencePos = state_manualZone;
		
		control_resetValveStates();
		control_resetSeedlingValve();
		state_valves[state_manualZone] = true;
	}
	else
	{
		if (control_checkTime())
		{
			//sequence start time
			//Serial.println("sequence start time");
			
			if (state_sequencePos == -1)
			{
				//sequence has not yet started:
				//alternate daily schedule
				//reset timers
				//start schedule
				
				//Serial.println("sequence has not yet started:");
				
				state_heavyDay = !state_heavyDay;
				
				Serial.print("state_heavyDay = ");
				Serial.println(state_heavyDay);
				
				if (state_heavyDay) 	timer_sequence = SimpleThread(setting_scheduleMinutesPerZoneHeavy * 60000);
				else 					timer_sequence = SimpleThread(setting_scheduleMinutesPerZoneLight * 60000);
				timer_sequence.reset();
				
				state_sequencePos = 0;
				
				Serial.print("state_sequencePos = 0");
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
				Serial.print("timer_sequence.check() == true");
				
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
				
				Serial.print("state_sequencePos = ");
				Serial.println(state_sequencePos);
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
				Serial.print("timer_seedlingInterval.check() == true");
				state_valves[7] = true;
				timer_seedlingTime.reset();
				state_seedlingWatering = true;
			}
		}
		else
		{
			if (timer_seedlingTime.check())
			{
				Serial.print("timer_seedlingTime.check() == true");
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
		Serial.println("control_checkTime = true");
		return true;
	}
	else if (time_now.hour() == (setting_scheduleStartHour + 6) && time_now.minute() == setting_scheduleStartMinute)
	{
		Serial.println("control_checkTime = true");
		return true;
	}
	//Serial.println("control_checkTime = false");
	return false;
}

void control_resetValveStates()
{
	Serial.println("control_resetValveStates");
	for (int i = 0; i < 7; i++)
	{
		state_valves[i] = false;
	}
}

void control_resetSeedlingValve()
{
	Serial.println("control_resetSeedlingValve");
	state_valves[7] = false;
}

void control_debugValveStates()
{
	Serial.println("Debugging valve states:");
	for (int i = 0; i < 8; i++)
	{
		Serial.print(i);
		Serial.print(" = ");
		Serial.println(state_valves[i]);
	}
}

void control_manualZoneIncrement()
{
	state_manualZone++;
	if (state_manualZone >= 8)
	{
		state_manualZone = 0;
	}
	Serial.print("state_manualZone = ");
	Serial.println(state_manualZone);
}

void control_manualPowerToggle()
{
	state_manualPower = !state_manualPower;
	if (!state_manualPower)
	{
		state_sequencePos = -1;
	}
	Serial.print("state_manualPower = ");
	Serial.println(state_manualPower);
	Serial.print("state_sequencePos = ");
	Serial.println(state_sequencePos);
}