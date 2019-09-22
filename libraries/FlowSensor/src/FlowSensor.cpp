#include "FlowSensor.h"


// ---------------------------------- PUBLIC


// Outside of class
FlowSensor *pointerToClass; // declare a pointer to testLib class

static void outsideInterruptHandler(void) { // define global handler
  pointerToClass->callback(); // calls class member handler
}

FlowSensor::FlowSensor(int _interruptPin, float _volumePerTick)
{
	pointerToClass = this; // assign current instance to pointer (IMPORTANT!!!)
	flowPin = _interruptPin;
	volumePerTick = _volumePerTick;
	pinMode(_interruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(_interruptPin), outsideInterruptHandler, CHANGE);
	lastTick = millis();
}

float FlowSensor::getFlowRate(void)
{
	return min(currentFlowRate, calcFlowRate());
}

float FlowSensor::getVolume(void)
{
	float ret = currentVolume;
	currentVolume = 0;
	return ret;
}


// ---------------------------- PRIVATE

void FlowSensor::callback(void)
{
	flowSensor = digitalRead(flowPin);
	if (millis() - lastTick > debounceTime)
	{
		//sufficient time has passed:
		
		
		if (flowSensor != flowSensorPrev)
		{
			//input is different to last logged value:
			
			lastTick = millis();
			
			currentFlowRate = calcFlowRate();
			currentVolume += volumePerTick;
		}
	}
	flowSensorPrev = flowSensor;
}

float FlowSensor::calcFlowRate()
{
	return (3600000 / (millis() - lastTick)) * volumePerTick;
}

