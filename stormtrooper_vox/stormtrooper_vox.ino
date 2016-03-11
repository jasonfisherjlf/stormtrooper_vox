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
AudioInputAnalog         adc1(A2);       //xy=278,409
AudioFilterStateVariable filter1;        //xy=478,407
AudioFilterStateVariable filter2;        //xy=670,412
AudioPlayMemory          playMem1;       //xy=677,282
AudioAnalyzePeak         peak1;          //xy=842,527
AudioMixer4              mixer1;         //xy=935,418
AudioOutputAnalog        dac1;           //xy=1163,420
AudioConnection          patchCord1(adc1, 0, filter1, 0);
AudioConnection          patchCord2(filter1, 0, filter2, 0);
AudioConnection          patchCord3(filter2, 2, mixer1, 1);
AudioConnection          patchCord4(filter2, 2, peak1, 0);
AudioConnection          patchCord5(playMem1, 0, mixer1, 0);
AudioConnection          patchCord6(mixer1, dac1);
// GUItool: end automatically generated code

const unsigned int* sounds[] = {
  AudioSample1,
  AudioSample2,
  AudioSample3,
  AudioSample4,
  AudioSample5,
  AudioSample6
};

void setup() {
  // put your setup code here, to run once:
  AudioMemory(16);
  mixer1.gain(1, 0);
  mixer1.gain(0, 0.90);
  filter1.frequency(1100);
//  filter1.resonance(1);
  filter2.frequency(500);
  filter2.resonance(0.71);
}

uint16_t wait = 500;
uint16_t dynWait = wait;
uint64_t triggerTime = 0;
boolean isTalking = true;
uint8_t soundNumber = 0;

void loop() {
  // put your main code here, to run repeatedly:
  if (peak1.available()) {
    if (peak1.read() > 0.2) {
      // Turn on voice
      mixer1.gain(1, 0.85);
      isTalking = true;
      triggerTime = millis();

      while (isTalking) {
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
      soundNumber = (soundNumber + 1) % 6;
//      playMem1.play(sounds[soundNumber]);
      playMem1.play(sounds[random(6)]);
    }
  }  
}
