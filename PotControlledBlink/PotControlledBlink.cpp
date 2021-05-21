#include "daisy_petal.h"

namespace terrarium
{
	class Terrarium
	{
		public:
			enum Sw
			{
				FOOTSWITCH_1 = 4,
				FOOTSWITCH_2 = 5,
				SWITCH_1 = 2,
				SWITCH_2 = 1,
				SWITCH_3 = 0,
				SWITCH_4 = 6
			};
	};
}
// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace terrarium;

// Declare a DaisySeed object called hardware
DaisyPetal hw;


int main(void)
{
    // Declare a variable to store the state we want to set for the LED.
    bool led_state;
    led_state = true;

    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hw.Init();

    // Loop forever
    hw.StartAdc();
    for(;;)
    {
        hw.ProcessAnalogControls();
        hw.ProcessDigitalControls();
        // Set the onboard LED
        // None of these switches work...
        if (hw.switches[Terrarium::SWITCH_2].Pressed() || 
                hw.switches[Terrarium::SWITCH_3].Pressed() || 
                hw.switches[Terrarium::SWITCH_4].Pressed()) {
            led_state = false;
            hw.seed.SetLed(false);
        } else {
            led_state = !led_state;
            hw.seed.SetLed(led_state);
            // XXX for some reason this is POT4 and not POT1 ... 
            float knob_1_val = hw.knob[1].Process();
            System::Delay(knob_1_val * 1000 + 100);
        }
        // Toggle the LED state for the next time around.

    }
}
