#include "xInstrument.h"

xInstrument instr;

void setup() {
  // put your setup code here, to run once
  instr.enableLEDS();
  instr.enableOD01();
  instr.enableSerial();
  instr.enableWiFi();
  instr.enableGoogleSheet();
  instr.enableAzure();
  instr.enableSensor("sw01");
  instr.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  instr.loop();

}
