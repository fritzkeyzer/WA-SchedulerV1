//mcp object
Adafruit_MCP23017 mcp;

//MCP outputs
const int mcpPin_valve0 = 0;
const int mcpPin_valve1 = 1;
const int mcpPin_valve2 = 2;
const int mcpPin_valve3 = 3;
const int mcpPin_valve4 = 4;
const int mcpPin_valve5 = 5;
const int mcpPin_valve6 = 6;
const int mcpPin_valve7 = 7;		//seedling zone

//MCP inputs
const int mcpPin_buttonA = 10;
const int mcpPin_buttonB = 11;
const int mcpPin_buttonC = 8;
const int mcpPin_buttonD = 9;

//M0 inputs
const int pin_moisture = A1;
const int pin_flow = 7;
const int pin_pressure = A0;

//Analog Calibration
float input_moistureMin = 410;
float input_moistureMax = 802;
float input_pressureCalLo = 109;				// raw digital value with zero added pressure
float input_pressureCalHi = 325;				// raw digital value with [input_pressureCalHiPressure] amount of added pressure
float input_pressureCalHiPressure = 2.45;		//bar value of input_pressureCalHi

//DotStar LED
Adafruit_DotStar px(1, 41, 40, DOTSTAR_BRG);
SimpleThread timer_heartbeatLED = SimpleThread(25);
int heartbeatLED_brightness = 0;
int heartbeatLED_increment = 5;
bool state_heartbeatLEDDirection = false;

//Buttons
bool state_buttonA_prev = false;
bool state_buttonB_prev = false;
bool state_buttonC_prev = false;
bool state_buttonD_prev = false;
const unsigned long debounceTimeButton = 100;		//ms
unsigned long lastPressedA;
unsigned long lastPressedB;
unsigned long lastPressedC;
unsigned long lastPressedD;

void io_setup()
{
	//set input pins
	pinMode(pin_moisture, INPUT_PULLUP);
	pinMode(pin_flow, INPUT_PULLUP);
	pinMode(pin_pressure, INPUT_PULLUP);
	
	mcp.begin(); // use default address 0
	mcp.pinMode(mcpPin_valve0, OUTPUT);
	mcp.pinMode(mcpPin_valve1, OUTPUT);
	mcp.pinMode(mcpPin_valve2, OUTPUT);
	mcp.pinMode(mcpPin_valve3, OUTPUT);
	mcp.pinMode(mcpPin_valve4, OUTPUT);
	mcp.pinMode(mcpPin_valve5, OUTPUT);
	mcp.pinMode(mcpPin_valve6, OUTPUT);
	mcp.pinMode(mcpPin_valve7, OUTPUT);
	mcp.pinMode(mcpPin_buttonA, INPUT);
	mcp.pinMode(mcpPin_buttonB, INPUT);
	mcp.pinMode(mcpPin_buttonC, INPUT);
	mcp.pinMode(mcpPin_buttonD, INPUT);
	mcp.pullUp(mcpPin_buttonA, HIGH); // turn on a 100K pullup internally
	mcp.pullUp(mcpPin_buttonB, HIGH);
	mcp.pullUp(mcpPin_buttonC, HIGH);
	mcp.pullUp(mcpPin_buttonD, HIGH);
	Serial.println("setup: mcp sucess");

	// Flow sensor stuff:
	flow_setup(pin_flow, setting_flowVolumePerTick);
	
	//DotStar
	px.begin(); // Initialize pins for output
	px.show();  // Turn all LEDs off ASAP
}

void io_input_update()
{
	uint16_t input_soilMoisture = analogRead(pin_moisture);
	uint16_t input_pressure = analogRead(pin_pressure);
	
	state_soilMoisture = (int) 100 * (1 - ((input_soilMoisture - input_moistureMin) / (input_moistureMax - input_moistureMin)));
	state_soilMoisture = min(max(state_soilMoisture, 0), 100);
	
	state_pressure = max(input_pressure - input_pressureCalLo, 0) * (input_pressureCalHiPressure/(input_pressureCalHi - input_pressureCalLo));
	state_pressure = min(max(state_pressure, 0), 16);
	
	//state_flowRate = flow_getFlowRate();
	state_flowVolume = flow_getVolume();

	io_buttons_update();
}

void io_output_update()
{
	mcp.digitalWrite(mcpPin_valve0, state_valves[0]);
	mcp.digitalWrite(mcpPin_valve1, state_valves[1]);
	mcp.digitalWrite(mcpPin_valve2, state_valves[2]);
	mcp.digitalWrite(mcpPin_valve3, state_valves[3]);
	mcp.digitalWrite(mcpPin_valve4, state_valves[4]);
	mcp.digitalWrite(mcpPin_valve5, state_valves[5]);
	mcp.digitalWrite(mcpPin_valve6, state_valves[6]);
	mcp.digitalWrite(mcpPin_valve7, state_valves[7]);
}

void io_buttons_update()
{
	state_buttonA = !mcp.digitalRead(mcpPin_buttonA);
	state_buttonB = !mcp.digitalRead(mcpPin_buttonB);
	state_buttonC = !mcp.digitalRead(mcpPin_buttonC);
	state_buttonD = !mcp.digitalRead(mcpPin_buttonD);
	
	if (millis() - lastPressedA > debounceTimeButton)
	{
		if (state_buttonA && !state_buttonA_prev)
		{
			lastPressedA = millis();
			settings_next();
		}
	}
	state_buttonA_prev = state_buttonA;
	
	if (millis() - lastPressedB > debounceTimeButton)
	{
		if (state_buttonB && !state_buttonB_prev)
		{
			lastPressedB = millis();
			settings_adjust();
		}
	}
	state_buttonB_prev = state_buttonB;
	
	if (millis() - lastPressedC > debounceTimeButton)
	{
		if (state_buttonC && !state_buttonC_prev)
		{
			lastPressedC = millis();
			control_manualPowerToggle();
		}
	}
	state_buttonC_prev = state_buttonC;

	if (millis() - lastPressedD > debounceTimeButton)
	{
		if (state_buttonD && !state_buttonD_prev)
		{
			lastPressedD = millis();
			control_manualZoneIncrement();
		}
	}
	state_buttonD_prev = state_buttonD;
}

void io_heartbeatLED_update()
{
	if (timer_heartbeatLED.check())
	{
		////index, G, R, B
		//if (state_heartbeatLED)
		//{
		//	px.setPixelColor(0, 150, 0, 0); // Set the pixel colors
		//	px.show();              // Refresh strip
		//	state_heartbeatLED = false;
		//}
		//else
		//{
		//	px.setPixelColor(0, 0, 0, 0); // Set the pixel colors
		//	px.show();              // Refresh strip
		//	state_heartbeatLED = true;
		//}

		if (state_heartbeatLEDDirection)
		{
			//UP:
			heartbeatLED_brightness += heartbeatLED_increment;

			if (heartbeatLED_brightness >= 255)
			{
				heartbeatLED_brightness = 255;
				state_heartbeatLEDDirection = false;
			}
		}
		else
		{
			//down:
			heartbeatLED_brightness -= heartbeatLED_increment;

			if (heartbeatLED_brightness <= 0)
			{
				heartbeatLED_brightness = 0;
				state_heartbeatLEDDirection = true;
			}
		}

		int r = 0;
		int g = 0;
		int b = 0;

		if (error_SD)
		{
			r = heartbeatLED_brightness;
			g = 0;
		}
		else
		{
			r = 0;
			g = heartbeatLED_brightness;
		}
		if (state_sequencePos != -1 || state_seedlingWatering)
		{
			b = 255 - heartbeatLED_brightness;
		}
		else
		{
			b = 0;
		}
		//index, G, R, B
		px.setPixelColor(0, g, r, b); // Set the pixel colors
		px.show();              // Refresh strip
	}
}