#ifndef _FRITZBUTTON_h_
#define _FRITZBUTTON_h_

#include <Arduino.h>




class FritzButton 
{

	public:
		FritzButton(int _pin, bool _normallyHigh);
		void attachPressedCallback(void _func(void));
		void update();
		
	private:
		
		int pin;
		bool normallyHigh;
		const unsigned long debounceTime = 100;		//ms
		volatile bool pressedPrev;
		volatile bool pressed;
		volatile unsigned long lastPressed;
		
		//callback
		typedef void (*callbackFuncType)(void);
		FritzButton::callbackFuncType pressedCallbackMethod;
		
		//methods:
		
		float calcFlowRate(void);
		
};

#endif
