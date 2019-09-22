#include "FritzButton.h"


// ---------------------------------- PUBLIC


FritzButton::FritzButton(int _pin, bool _normallyHigh)
{
	pin = _pin;
	normallyHigh = _normallyHigh;
	pinMode(pin, INPUT_PULLUP);
}

void FritzButton::attachPressedCallback(void _func(void))
{
	pressedCallbackMethod = _func;
}

void FritzButton::update()
{
	if (normallyHigh) 	pressed = !digitalRead(pin);
	else				pressed = digitalRead(pin);
	
	if (millis() - lastPressed > debounceTime)
	{
		if (pressed && !pressedPrev)
		{
			lastPressed = millis();
			pressedCallbackMethod();
		}
	}
	pressedPrev = pressed;
}


// ---------------------------- PRIVATE


