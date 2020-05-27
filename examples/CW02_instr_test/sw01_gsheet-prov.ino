#include "xInstrument.h"
#include "xSW01.h"


double tempC, press, hum;
xInstrument instr;
xSW01 SW01;

void setup() {
  // put your setup code here, to run once

  instr.createVariables(3, "temp", "pressure","humidity");
  instr.enableLEDS();
  instr.enableWiFi();
  instr.enableGoogleSheet();
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

  instr.updateVariables(3, tempC,  press, hum);
  instr.loop();
  

}
