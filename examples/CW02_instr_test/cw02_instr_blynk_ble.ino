#include "xInstrument.h"

xInstrument instr;

void setup() {
  // put your setup code here, to run once
  instr.enableLEDS();
  instr.enableOD01();
  instr.enableSensor("sw01");
  instr.enableSensor("sl01");
  instr.enableSensor("sw10");
  instr.enableSensor("sw12");
  instr.enableBlynkBLE();
  instr.enableSerial();
  instr.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  instr.loop();

}
