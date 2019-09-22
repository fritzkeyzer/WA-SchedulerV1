#ifndef _FLOWSENSOR_h_
#define _FLOWSENSOR_h_

#include <Arduino.h>




class FlowSensor 
{

	public:
		FlowSensor(int _interruptPin, float _volumePerTick);
		float getFlowRate(void);
		float getVolume(void);
		void callback(void);
		
		
	private:
		
		const unsigned long debounceTime = 50;		//ms
		float volumePerTick;
		volatile float currentFlowRate;
		volatile float currentVolume;
		int flowPin;
		volatile bool flowSensor;
		volatile bool flowSensorPrev;
		volatile unsigned long lastTick;
		
		//methods:
		
		float calcFlowRate(void);
		
};

#endif
