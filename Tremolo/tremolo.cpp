#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"
#include "Synthesis/oscillator.h"

using namespace daisy;
using namespace terrarium;

DaisyPetal hardware;
daisysp::Oscillator osc;


/**
 * Features:
 *
 * POT1 - Waveform: Sin, Tri, Saw, Ramp, and Square
 * POT2 - Depth:    0 - 100%
 * POT3 - Rate:     0.1 - 10 Hz (make logarithmic)
 *
 * Future:
 *  - Double tremolo?
 */

uint16_t max_freq = 10;
float freq;
float amplitude;
uint8_t waveform = daisysp::Oscillator::WAVE_SIN;

void Controls() {
    freq = hardware.knob[Terrarium::KNOB_1].Process() * max_freq;
    amplitude = hardware.knob[Terrarium::KNOB_2].Process();
}

void AudioCallback(float **in, float **out, size_t size) {
    Controls();


    osc.SetFreq(freq);
    osc.SetAmp(amplitude);
    osc.SetWaveform(daisysp::Oscillator::WAVE_SIN);

    float dry_l;
    float dry_r;
    float wave_out;

    // Signal interleaved; left, right, left, right
    for (size_t ii = 0; ii < size; ii++) {
        dry_l = in[0][ii];

        wave_out = osc.Process();

        out[0][ii] = dry_l * (1 - wave_out);
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
        // Do something
    }
    return 0;
}
