#include "daisy_petal.h"
#include "terrarium.h"
using namespace daisy;
using namespace terrarium;

DaisyPetal petal;

void switch_led_control() {
    while(1) {
        petal.ProcessAllControls();  // this is not needed?

        bool pressed = petal.switches[Terrarium::SWITCH_1].Pressed();
        if (pressed) {
            petal.seed.SetLed(true);
        } else {
            petal.seed.SetLed(false);
        }

        System::Delay(10);
    }
}

void pot_led_flash_control(uint8_t pot_num) {
    bool led_state = true;
    float knob_val;

    while (1) {
        petal.seed.SetLed(led_state);
        led_state = !led_state;

        petal.knob[pot_num].Process();
        knob_val = petal.knob[pot_num].Value();

        uint32_t ms = knob_val * 1000 + 50;
        System::Delay(ms);
    }
}

int main(void)
{

    petal.Init();
    petal.StartAdc();
    
    // petal.KNOB_1 = POT_1
    // petal.KNOB_2 = POT_4
    // petal.KNOB_3 = POT_2
    // petal.KNOB_4 = POT_5
    // petal.KNOB_5 = POT_3
    // petal.KNOB_6 = POT_6

    pot_led_flash_control(petal.KNOB_6);

    return 0;
}
