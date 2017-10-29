/* By Ross Fish 2017
    clock as slow as 1min & 48sec and as fast as 5ms
*/

#include <elapsedMillis.h>
unsigned int clockDuration = 0;

int clockCoarse = 0;
int clockFine = 0;
int pw = 0;
int masterCount = 0;
int upDown = 0;
boolean transportSwitch[2];

int modArray[5];
int outs[5] = {8, 9, 10, A3, A4};

elapsedMillis sinceClock;
elapsedMillis sinceOut[5];
elapsedMillis sinceReset;

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
}

void loop() {

  upDown = digitalRead(4);

  if (digitalRead(5) == HIGH)  // even odd switch
    for (int i = 0; i < 5; i++)
    {
      modArray[0] = 3;
      modArray[1] = 5;
      modArray[2] = 1; // straight clock
      modArray[3] = 7;
      modArray[4] = 9;
    }
  else {
    for (int i = 0; i < 5; i++)
    {
      modArray[0] = 2;
      modArray[1] = 4;
      modArray[2] = 1; // straight clock
      modArray[3] = 6;
      modArray[4] = 8;
    }
  }

  if (digitalRead(6) == HIGH) // divide all by 4 -- max divide switch
    for (int i = 0; i < 5; i++)
    {
      modArray[i] *= 4;
      modArray[2]  = 1; // straight clock
    }

  if (digitalRead(2) == HIGH)
    clockCoarse = map(analogRead(A0), 0, 1023, 500, 25);
  else
    clockCoarse = map(analogRead(A0), 0, 1023, 3000, 500);

  clockFine = map(analogRead(A2), 0, 1023, 15, -15);

  clockDuration = clockCoarse + clockFine;
  transportSwitch[1] = transportSwitch[0];
  transportSwitch[0] = digitalRead(3);
  if (transportSwitch[0] > transportSwitch[1]) //stop to start transition
  {
    masterCount = 0;
    sinceReset = 0;
  }
  if (sinceReset <= pw)
    digitalWrite(7, HIGH);
  else
    digitalWrite(7, LOW);
  if (sinceClock > clockDuration && digitalRead(3) == HIGH) { // run/stop switch
    masterCount++;
    sinceClock = 0;
  }

  //if (digitalRead(4) == HIGH)

  for (int i = 0; i < 5; i++)
  {
    switch (upDown)
    {
      case 0:
        if (masterCount % modArray[i] == 0)
          sinceOut[i] = 0;
        break;
      case 1:
        if ((masterCount-1) % modArray[i] == 0)
          sinceOut[i] = 0;
        break;
    }
  }

  pw = (clockDuration * (analogRead(A1) / 2046.) + 10) ; // change to 1023 for 100% PW range

  if (sinceClock < pw)
  {
    for (int i = 0; i < 5; i++)
    {
      switch(upDown)
      {
        case 0:
      if (masterCount % modArray[i] == 0)
        digitalWrite(outs[i], HIGH);
        break;
        case 1:
        if ((masterCount-1) % modArray[i] == 0)
        digitalWrite(outs[i], HIGH);
        break;
      }
    }
  }
  else
  {
    for (int i = 0; i < 5; i++)
    {
      if (sinceOut[i] < (pw * modArray[i]))
        digitalWrite(outs[i], LOW);
    }
  }
}




