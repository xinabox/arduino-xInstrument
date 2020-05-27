#include "xInstrument.h"
#include "xSW01.h"


double tempC, press, hum;
xInstrument instr;
xSW01 SW01;

void setup() {
  // put your setup code here, to run once

  // (no_of_vars, var1, var2, ...
  instr.createVariables(3, "temp", "pressure","humidity");
  instr.enableLEDS();
  instr.enableOD01();
  instr.begin();
  Wire.begin();
  SW01.begin();
  instr.enableSerial();

}

void loop() {
  // put your main code here, to run repeatedly:
  SW01.poll();
  
  tempC = SW01.getTempC();
  press = SW01.getPressure();
  hum = SW01.getHumidity();

  // update variables with sensor values
  instr.updateVariables(3, tempC, press, hum);
  instr.loop();
  

}
