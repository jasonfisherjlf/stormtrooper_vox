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
AudioInputAnalog         adc1(A2);           //xy=278,379
AudioMixer4              mixerAdc;         //xy=429,398
AudioFilterStateVariable filter1;        //xy=590,405
AudioMixer4              mixerVoice;         //xy=727,407
AudioPlayMemory          playMem1;       //xy=806,313
AudioMixer4              mixerFinal;         //xy=884,413
AudioAnalyzePeak         peak1;          //xy=886,480
AudioOutputAnalog        dac1;           //xy=1017,413
AudioConnection          patchCord1(adc1, 0, mixerAdc, 0);
AudioConnection          patchCord2(mixerAdc, 0, filter1, 0);
AudioConnection          patchCord3(filter1, 1, mixerVoice, 0);
AudioConnection          patchCord4(mixerVoice, 0, mixerFinal, 1);
AudioConnection          patchCord5(mixerVoice, peak1);
AudioConnection          patchCord6(playMem1, 0, mixerFinal, 0);
AudioConnection          patchCord7(mixerFinal, dac1);
// GUItool: end automatically generated code

const unsigned int* sounds[] = {
  AudioSample1,
  AudioSample2,
  AudioSample3,
  AudioSample4,
  AudioSample5,
  AudioSample6
};

const uint16_t WAIT = 400;
const float TALK_START_LVL = 0.15;
const float TALK_STOP_LVL  = 0.14;
const float VOICE_LVL = 0.85;
const float PLAY_LVL  = 0.80;

uint16_t dynWait = WAIT;
uint64_t triggerTime = 0;
boolean isTalking = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  AudioMemory(16);
  mixerAdc.gain(0, 0.05);
  mixerVoice.gain(0, 4.0);
  mixerFinal.gain(1, 0);
  mixerFinal.gain(0, 0.95);
  filter1.frequency(1900);
  filter1.resonance(2.0);
}

void loop() {
  if (peak1.available()) {
    if (peak1.readPeakToPeak() > TALK_START_LVL) {
      // Turn on voice
      mixerFinal.gain(1, VOICE_LVL);
      isTalking = true;
      triggerTime = millis();

      while (isTalking) {
        // Increase dynamic wait
        if (millis() > triggerTime + 1500) { dynWait = 600; }
        // Check if they have stopped talking
        while (!peak1.available());
        if (peak1.readPeakToPeak() < TALK_STOP_LVL) {
          delay(dynWait);
          while (!peak1.available());
          if (peak1.readPeakToPeak() < TALK_STOP_LVL) { isTalking = false; }
        }
      }

      // RESET to default and play click
      dynWait = WAIT;
      mixerFinal.gain(1, 0);
      playMem1.play(sounds[random(6)]);
      delay(playMem1.lengthMillis() + 100);
      // Clear peak1
      peak1.read();
    }
  }  
}
