#include <math.h>
#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"
#include "Synthesis/oscillator.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;

DaisyPetal hardware;
daisysp::Oscillator osc;


/**
 * Features:
 *
 * POT1 - Rate:     0.1 - 10 Hz (make logarithmic)
 * POT2 - Depth:    0 - 100%
 * POT3 - Waveforms:
 *  - 0-25%:    Sine (STATIC)
 *  - 26-50%:   Triangle (STATIC)
 *  - 51-65%:   Triangle (DYNAMIC)
 *  - 66-100%:  Sine (DYNAMIC)
 *  Notes: 
 *  - Ramp and Saw didn't sound pleasing to me, so I removed them.
 *  - STATIC: The full range of the attenuation is applied.
 *  - DYNAMIC: The range of attenuation applied depends on input amplitude.
 *
 * Future:
 *  - Double tremolo?
 * 
 * Investigation:
 *  - Audio in is: -1 to +1
 *  - Oscillator out is: -AMP to +AMP
 */


// XXX: Put this in a class?

// Constants:
enum ATTENUATION_MODES {
    STATIC,
    DYNAMIC
};
float MIN_FREQUENCY = 0.5;
float MAX_FREQUENCY = 8.0;
uint8_t KNOB_RESOLUTION = 255;
uint8_t NUM_MODES = 4;
uint8_t WAVEFORM_MODE_MAP[4] = {
    Oscillator::WAVE_SIN,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_SIN
};
uint8_t ATTENUATION_MODE_MAP[4] = {
    STATIC,
    STATIC,
    DYNAMIC,
    DYNAMIC    
};

// Globals:
float frequency;
float amplitude;
uint8_t waveform;
uint8_t mode;


float logKnob(float input, float min, float max, uint16_t resolution) {
    // input is assumed to be normalized: 0 - 1
    // XXX I am not sure this is correct. Think about it later.
    float log_range = pow(input * resolution + 1, 2);
    float log_range_normalized = log_range / pow((resolution + 1), 2);
    return ((max - min) * log_range_normalized) + min;
}

void Controls() {
    // XXX This doesn't need to set globals, can return values.
    // Not sure if there is a plus/minus here.
    float knob1val = hardware.knob[Terrarium::KNOB_1].Process();
    float knob2val = hardware.knob[Terrarium::KNOB_2].Process();
    float knob3val = hardware.knob[Terrarium::KNOB_3].Process();

    int mode_select = (int)(knob3val * NUM_MODES);

    frequency = logKnob(knob1val, MIN_FREQUENCY, MAX_FREQUENCY, KNOB_RESOLUTION);
    amplitude = knob2val;
    waveform = WAVEFORM_MODE_MAP[mode_select];
    mode = ATTENUATION_MODE_MAP[mode_select];
}

void AudioCallback(float **in, float **out, size_t size) {
    Controls();

    // XXX Do we want to see if a change has occured and only update then?
    // Not sure of the cost of resetting frequency, amp, etc..
    osc.SetFreq(frequency);
    osc.SetAmp(amplitude);
    osc.SetWaveform(waveform);

    float dry_l;
    float dry_amp;
    float raw_wave;
    float positive_wave;
    float difference_wave;

    for (size_t ii = 0; ii < size; ii++) {
        dry_l = in[0][ii];
        dry_amp = ((dry_l + 1) / 2) * 1.7;  // XXX 1.7 is arbitrary.
        // assert dry_amp >= 0 && dry_amp <= 1;
        
        // Given an amplitude of 1, wave is -1 to +1
        raw_wave = osc.Process();

        // Move wave into positive range. 2 because it is max pk-pk
        positive_wave = (raw_wave + amplitude) / 2;
        // assert positive_wave <= 1 && positive_wave >= 0;
 
        if (mode == DYNAMIC) {
             positive_wave = positive_wave * dry_amp;
        }

        // Tester...
        /* if (positive_wave > 1 || positive_wave < 0) { */
        /*     hardware.seed.SetLed(true); */
        /* } */

        // Always situate the sine wave at top
        difference_wave = 1 - positive_wave;
        // assert difference_wave <= 1 && difference_wave >= 0;

        // write out
        out[0][ii] = dry_l * difference_wave;

    }
}

int main() {
    hardware.Init();

    float sample_rate = hardware.AudioSampleRate();
    osc.Init(sample_rate);
    
    hardware.StartAdc();
    hardware.StartAudio(AudioCallback);
    while(1) {
        System::Delay(10);
    }
    return 0;
}
