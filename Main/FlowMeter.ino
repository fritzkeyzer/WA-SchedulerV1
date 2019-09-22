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
			currentFlowRate = flow_calcFlowRate();
			currentVolume += volumePerTick;
			lastTick = millis();
			Serial.println("pulse");
		}
	}
	flowSensorPrev = flowSensor;
}