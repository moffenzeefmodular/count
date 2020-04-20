// SWARM
// Designed by Ross Fish 
// Programmed by Ben Davis 

#include <elapsedMillis.h>

elapsedMillis sinceClock;

int lowOscRange[2] = {40, 200};  // VCO Low range
int hiOscRange[2] = {1000, 15000}; // VCO high range 

int clkLow[2] = {300, 30}; // Main clock slow speeds
int clkHigh[2] = {1000, 300}; // Main clock fast speeds

float smoothCoarse = 1;
float smoothFine = 1;

int currentOsc = 0; //current state of sqr oscillator
int lastOsc = 0; //last state of square oscillator

int highLowRange = 1;
float width = 0.5;

float oscFreq = 100;

float phaccu = 0;

const int outs[4] = {8, 9, A3, A4};

class Chirp
{
  private:
    elapsedMillis sinceStep;
    float currentCount;
    unsigned long lastCount;
    int lastFreeze;
    float widthHold;
    int currentFreeze;
    int lastStep;
    int currentStep;
    int currentState = 0;
    int lastState = 0;
    int seqLength = 16;
    int gates[64];
  public:
    float timeRatio;
    int freeze = 0;
    Chirp()
    {
      for (uint8_t i = 0; i < 64; i++)
      {
        gates[i] = rand() % 2;
      }
    }

    int update(float rt)
    {
      const unsigned long msCount = rt * timeRatio;
      lastFreeze = currentFreeze;
      currentFreeze = freeze;
      if (currentFreeze > lastFreeze)
      {
        seqLength = (rand() % 64) + 1;
      }
      if (sinceStep >= msCount)
      {
        currentStep++;
        if (currentStep > seqLength)
          currentStep = 0;
        if (freeze == 0)
        {
          const int density = 20; // density of notes 0-100%
          gates[rand() % seqLength] = (rand() % 100 <= density);
        }
        sinceStep -= msCount;
      }
      if ((sinceStep < ((float)msCount * width)) && gates[currentStep] == 1)
      {
        return (1);
      }
      else
      {
        return (0);
      }
    }
};

Chirp chirp[4];

void setup() {

  pinMode(2, INPUT); // range switch
  pinMode(3, INPUT); // run/stop switch
  pinMode(4, INPUT); // up/down switch
  pinMode(5, INPUT); // even/odd switch
  pinMode(6, INPUT); // max div time switch

  pinMode(7, OUTPUT); // reset out
  pinMode(8, OUTPUT); // /n out 1
  pinMode(9, OUTPUT); // /n out 2
  pinMode(10, OUTPUT); // main out
  pinMode(A3, OUTPUT); // /n out 3
  pinMode(A4, OUTPUT); // /n out 4

  pinMode(A5, OUTPUT); // SMT power indicator
  digitalWrite(A5, HIGH); // turno n power indicator

   // Timing intervals
   chirp[0].timeRatio=1.3; //  
   chirp[1].timeRatio=3.7; // 
   chirp[2].timeRatio=5.5; // 
   chirp[3].timeRatio=7.9; // 
}

void loop() {
  chirp[0].freeze = digitalRead(3);
  chirp[1].freeze = digitalRead(4);
  chirp[2].freeze = digitalRead(5);
  chirp[3].freeze = digitalRead(6);

  const int lowHighRange = digitalRead(2);

  const int coarseKnob = analogRead(A0);
  const int fineKnob = analogRead(A2);
  const int widthKnob = analogRead(A1);

  smoothCoarse += (coarseKnob - smoothCoarse) * 0.2f;
  smoothFine += (fineKnob - smoothFine) * 0.2f;

  const float finalPitch = map(smoothCoarse, 0, 1000, 20, 980) + map(smoothFine, 0, 1000, -20, 20);

  const unsigned long clkSpeed = map(finalPitch, 1000, 0, clkLow[lowHighRange], clkHigh[lowHighRange]);

  width = map(widthKnob, 0, 1000, 1, 50) * 0.01f;

  for (uint8_t i = 0; i < 4; i++)
  {
    digitalWrite(outs[i], chirp[i].update(clkSpeed));
  }

  if (sinceClock > clkSpeed)
  {
    sinceClock -= clkSpeed;
  }

  if (sinceClock < clkSpeed * width)
  {
    digitalWrite(10, HIGH);
  }
  else
  {
    digitalWrite(10, LOW);
  }

  tone(7, map(finalPitch, 0, 1000, lowOscRange[lowHighRange], hiOscRange[lowHighRange]));
}
