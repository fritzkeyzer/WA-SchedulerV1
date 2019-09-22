//Pins
const int pin_valve0 = 12;
const int pin_valve1 = 11;
const int pin_valve2 = 10;
const int pin_valve3 = 9;
const int pin_valve4 = 7;
const int pin_valve5 = 5;
const int pin_valve6 = 4;
const int pin_valve7 = 13;		//disconnected

const int pin_moisture = A0;
const int pin_flow = A1;
const int pin_pressure = A3;
const int pin_buttonA = A2;
const int pin_buttonB = A4;
const int pin_buttonC = A5;
const int pin_buttonD = 3;

//faceplate buttons
FritzButton button_a(pin_buttonA, true);
FritzButton button_b(pin_buttonB, true);
FritzButton button_c(pin_buttonC, true);
FritzButton button_d(pin_buttonD, true);

//settings
float input_moistureMin = 410;
float input_moistureMax = 802;
float input_pressureCalLo = 109;				// raw digital value with zero added pressure
float input_pressureCalHi = 325;				// raw digital value with [input_pressureCalHiPressure] amount of added pressure
float input_pressureCalHiPressure = 2.45;		//bar value of input_pressureCalHi

//FlowSensor flowSensor(pin_flow, flow_volumePerTick);

SimpleThread timer_hour(3600000);

//DotStar LED
Adafruit_DotStar px(1, 41, 40, DOTSTAR_BRG);
uint8_t red = 0, green = 0, blue = 0;

void io_setup()
{
	//set output pins
	pinMode(pin_valve0, OUTPUT);
	pinMode(pin_valve1, OUTPUT);
	pinMode(pin_valve2, OUTPUT);
	pinMode(pin_valve3, OUTPUT);
	pinMode(pin_valve4, OUTPUT);
	pinMode(pin_valve5, OUTPUT);
	pinMode(pin_valve6, OUTPUT);
	pinMode(pin_valve7, OUTPUT);
	
	pinMode(pin_moisture, INPUT_PULLUP);
	pinMode(pin_flow, INPUT_PULLUP);
	pinMode(pin_pressure, INPUT_PULLUP);
	pinMode(pin_buttonA, INPUT_PULLUP);
	pinMode(pin_buttonB, INPUT_PULLUP);
	pinMode(pin_buttonC, INPUT_PULLUP);
	pinMode(pin_buttonD, INPUT_PULLUP);
	
	//buttons:
	button_a.attachPressedCallback(settings_next);
	button_b.attachPressedCallback(settings_adjust);
	button_c.attachPressedCallback(control_manualPowerToggle);
	button_d.attachPressedCallback(control_manualZoneIncrement);
	
	
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
	//state_pressure = input_pressure;
	//state_pressure = (int)  (1 - ((input_pressure - input_pressureMin) / (input_pressureMax - input_pressureMin)));
	//state_pressure = min(max(state_pressure, 0), 100);
	
	state_flowRate = flow_getFlowRate();
	state_flowVolume = flow_getVolume();
	
	
	//Serial.print("volume: ");
	//Serial.print(state_flowHour);
	//
	//Serial.print(" | rate: ");
	//Serial.println(state_flowRate);
	
	//buttone
	button_a.update();
	button_b.update();
	button_c.update();
	button_d.update();
}

void io_output_update()
{
	digitalWrite(pin_valve0, state_valves[0]);
	digitalWrite(pin_valve1, state_valves[1]);
	digitalWrite(pin_valve2, state_valves[2]);
	digitalWrite(pin_valve3, state_valves[3]);
	digitalWrite(pin_valve4, state_valves[4]);
	digitalWrite(pin_valve5, state_valves[5]);
	digitalWrite(pin_valve6, state_valves[6]);
	digitalWrite(pin_valve7, state_valves[7]);
	
	
	//set dotstar color to soil moisture
	red = 255 - (state_soilMoisture * 2.55);
	green = 255 - red;
	blue = 255 - red;
	
	red 	*= 0.3;
	blue 	*= 0.3;
	green 	*= 0.3;
	px.setPixelColor(0, green, red, blue); // Set the pixel colors
	px.show();              // Refresh strip
}
