#include "Arduino.h"
#include "cw02_instrument.h"

xInstrument::xInstrument()
{
}


// Enable OD01 OLED Display
bool xInstrument::enableOD01()
{
  Wire.begin();
  if (!xCore.ping(OD01_I2C_ADDRESS))
  {
    return false;
  }

  OD01_enabled = true;
  OD01.begin();
  OD01.clear();
  OD01.println("Hello");
  return true;
}

// Enable Serial port @BR 115200
bool xInstrument::enableSerial()
{
  Serial.begin(115200);

  //Serial_enabled = true;

  return true;
}

// Enable XinaBox sensors
bool xInstrument::enableSensor(String sensor)
{
  // Make string lower case
  sensor.toLowerCase();

  //OD01.println(sensor);

  // SW01 sensor
  if (sensor == String("sw01"))
  {

    OD01.println("Hello SW01");
    if (xCore.ping(BME280_I2C_ADDRESS))
    {
      SW01.begin();
      ACK_SW01 = true;
    } else {
      ACK_SW01 = false;
      return false;
    }
  }

  // SW03 sensor
  if (sensor == String("sw03"))
  {
    if (xCore.ping(MPL3115A2_I2C_ADDRESS))
    {
      SW03.begin();
      ACK_SW03 = true;
    } else {
      ACK_SW03 = false;
      return false;
    }
  }

  // SW10 sensor
  if (sensor == String("sw10"))
  {
    if (xCore.ping(LM75B_I2C_ADDR))
    {
      SW10.begin();
      ACK_SW10 = true;
    } else {
      ACK_SW10 = false;
      return false;
    }
  }

  // SW12 sensor
  if (sensor == String("sw12"))
  {
    if (xCore.ping(SW12_I2C_ADDR))
    {
      SW12.begin();
      ACK_SW12 = true;
    } else {
      ACK_SW12 = false;
      return false;
    }
  }

  return true;
}

bool xInstrument::enableWiFi()
{
  WiFi_enabled  = true;
}

bool xInstrument::enableBLE()
{
  BLE_enabled  = true;
}

bool xInstrument::enableGoogleSheet()
{
  Gsheet_enabled  = true;
}

bool xInstrument::enableAzure()
{
  Azure_enabled  = true;
}

bool xInstrument::enableAWS()
{
  AWS_enabled  = true;
}

bool xInstrument::enableSPIFFS()
{
  SPIFFS_enabled  = true;
}

bool xInstrument::begin()
{

  if (WiFi_enabled || BLE_enabled || Gsheet_enabled || AWS_enabled || Azure_enabled)
  {
    prv.begin();

    if (WiFi_enabled)
    {
		
      //prv.nameJsonFile(uName);
      prv.addVariable("WiFi_SSID", "yourNetworkName");
      prv.addVariable("WiFi_PSK", "yourNetworkPassword");
    }
	
	if(Gsheet_enabled)
	{
	  prv.addVariable("GAS_ID", "GAS_ID");
	}

	prv.transmit();
	//Writes this string, with the CRLF and SYNC prefixed and CRLF suffixed

	prv.receive();
	//Reads this string, with CRLF suffixed

    if (prv.success())
    {
      if (WiFi_enabled)
      {
        prv.getVariable("WiFi_SSID", ssid);
        prv.getVariable("WiFi_PSK", password);
      }
	  
	  if(Gsheet_enabled)
	  {
		prv.getVariable("GAS_ID",  gas);
	  }

      if (Serial_enabled)
      {
        Serial.println(ssid);
        Serial.println(password);
      }

      if (WiFi_enabled)
      {
        WiFi.begin(ssid.c_str(), password.c_str());

        while (WiFi.status() != WL_CONNECTED) {
          digitalWrite(LED_RED, HIGH);
          delay(250);
          digitalWrite(LED_RED, LOW);
          delay(250);
        }
      }

      if (Serial_enabled)
      {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
      }

      if (WiFi_enabled)
      {
        digitalWrite(LED_GREEN, HIGH);
      }
    }
    else
    {
      prv.fail();
    }
  }
}

void xInstrument::sendData(String params) {
   HTTPClient http;
   String url="https://script.google.com/macros/s/"+gas+"/exec?"+params;
   
   if(Serial_enabled)
   {
	Serial.print(url);
    Serial.print("Making a request");
   }
    http.begin(url, root_ca); //Specify the URL and certificate
    int httpCode = http.GET();  
    http.end();
	
	if(Serial_enabled)
	{
		Serial.println(": done "+httpCode);
	}
}

void xInstrument::loop()
{
	
  if(ACK_SW01)
  {
	  SW01.poll();
	  SW01_temp = SW01.getTempC();
	  SW01_press = SW01.getPressure();
	  SW01_humidity = SW01.getHumidity();
  }
	
  if (OD01_enabled)
  {
	OD01.clear();
    if (ACK_SW01)
    {
      OD01.println("SW01_Temp_" + String(SW01_temp) + "_C");
      OD01.println("SW01_Press_" + String(SW01_press / 100.0) + "_hPa");
      OD01.println("SW01_Hum_" + String(SW01_humidity) + "_%RH");
    }
  }

  if (Serial_enabled)
  {
    if (ACK_SW01)
    {
      Serial.println("SW01_Temp_" + String(SW01_temp) + "_C");
      Serial.println("SW01_Press_" + String(SW01_press / 100.0) + "_hPa");
      Serial.println("SW01_Hum_" + String(SW01_humidity) + "_%RH");
    }
  }

  delay(2000);
  
  // Send data to Gsheet
  if(Gsheet_enabled && WiFi_enabled)
  {
	  gsheet_string = "timestamp=" + String(millis());
	  
	  if(ACK_SW01)
	  {
		  gsheet_string += "&sw01_temp=" + String(SW01_temp);
		  gsheet_string += "&sw01_press=" + String(SW01_press);
		  gsheet_string += "&sw01_hum=" + String(SW01_humidity);
		  
		  // Send Data to gsheet
		  sendData(gsheet_string);
	  }
  }
  
}