#include "xInstrument.h"
#include "xSW01.h"
#include "xSL01.h"


double tempC, press, hum;
double lux, uva, uvb;
xInstrument instr;
xSW01 SW01;
xSL01 SL01;

void setup() {
  // put your setup code here, to run once

  // Create variables to store sensor values
  // (no_of_vars, var1, var2, ...)
  instr.createVariables(6, "temp", "pressure","humidity", "lux", "uva", "uvb");
  instr.enableLEDS();
  instr.begin();
  Wire.begin();
  SW01.begin();
  SL01.begin();
  instr.enableSerial();

}

void loop() {
  // put your main code here, to run repeatedly:
  SW01.poll();
  SL01.poll();
  
  tempC = SW01.getTempC();
  press = SW01.getPressure();
  hum = SW01.getHumidity();
  lux = SL01.getLUX();
  uva = SL01.getUVA();
  uvb = SL01.getUVB();
  
  // update variables with sensor values
  instr.updateVariables(6, tempC, press, lux, uva, uvb);
  instr.loop();
  

}
