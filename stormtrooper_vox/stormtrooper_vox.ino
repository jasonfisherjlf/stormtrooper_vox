#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "AudioSample1.h"
#include "AudioSample2.h"
#include "AudioSample3.h"
#include "AudioSample4.h"
#include "AudioSample5.h"
#include "AudioSample6.h"

// GUItool: begin automatically generated code
AudioInputAnalog         adc1(A2);     //xy=75.45454406738283,216.3636360168457
AudioAnalyzeFFT256       fft256_1; //xy=202,342
AudioFilterBiquad        biquad1;  //xy=264.2727165222168,218.09091663360596
AudioFilterStateVariable filter1;  //xy=441.00000381469727,225.27273178100586
AudioPlayMemory          playMem1; //xy=596,90
AudioFilterStateVariable filter2;  //xy=614,231
AudioAnalyzePeak         peak1;    //xy=761,335
AudioMixer4              mixer1;   //xy=854,226
AudioOutputAnalog        dac1;     //xy=1082,228
AudioConnection          patchCord1(adc1, biquad1);
AudioConnection          patchCord2(adc1, fft256_1);
AudioConnection          patchCord3(biquad1, 0, filter1, 0);
AudioConnection          patchCord4(filter1, 0, filter2, 0);
AudioConnection          patchCord5(playMem1, 0, mixer1, 0);
AudioConnection          patchCord6(filter2, 2, mixer1, 1);
AudioConnection          patchCord7(filter2, 2, peak1, 0);
AudioConnection          patchCord8(mixer1, dac1);
// GUItool: end automatically generated code

const unsigned int* sounds[] = {
  AudioSample1,
  AudioSample2,
  AudioSample3,
  AudioSample4,
  AudioSample5,
  AudioSample6
};

uint16_t wait = 500;
uint16_t dynWait = wait;
uint64_t triggerTime = 0;
boolean isTalking = true;
uint16_t freqStart = 500;
uint16_t freqEnd = 1000;
float fftVal = 0.0;
float fftPrev = 0.0;
float fftThresh = 10.0; // 500% spike
// 172Hz steps: fft256, 43Hz steps: fft1024
uint16_t fftStart = (freqStart / 172) - 15;
uint16_t fftEnd = (freqEnd / 172) + 15;

void setup() {
  // put your setup code here, to run once:
  AudioMemory(16);
  mixer1.gain(1, 0);
  mixer1.gain(0, 0.90);
  filter1.frequency(freqEnd);
//  filter1.resonance(1);
  filter2.frequency(freqStart);
  filter2.resonance(0.71);
  biquad1.setNotch(0, 800, .7);
//  biquad1.setNotch(1, 400, 100);
//  biquad1.setNotch(2, 400, 100);
//  biquad1.setNotch(3, 400, 100);
}

void cancelFeedback() {
  fftVal = fft256_1.read(0,127);
  for (int i=0; i<fftEnd; i++) {
    fftPrev = fftVal;
    fftVal = fft256_1.read(i);
    if (fftVal / fftPrev > fftThresh) {
      mixer1.gain(1, 0.0);
      delay(100);
      mixer1.gain(1, 0.85);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (peak1.available()) {
    if (peak1.read() > 0.2) {
      // Turn on voice
      mixer1.gain(1, 0.85);
      isTalking = true;
      triggerTime = millis();

      while (isTalking) {
        if (fft256_1.available()) {
          cancelFeedback();
        }
        if (millis() > triggerTime + 2500) { dynWait = 1000; }
        while (!peak1.available());
        if (peak1.read() < 0.2) {
          delay(dynWait);
          if (peak1.read() < 0.2) { isTalking = false; }
        }
      }
      dynWait = wait;
      // Turn off voice
      mixer1.gain(1, 0);
      playMem1.play(sounds[random(6)]);
    }
  }  
}
