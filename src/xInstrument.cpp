#include "Arduino.h"
#include "xInstrument.h"

uint8_t xInstrument::ACK_SW01 = 0;
uint8_t xInstrument::ACK_SW10 = 0;
uint8_t xInstrument::ACK_SW12 = 0;
uint8_t xInstrument::ACK_SL01 = 0;

float xInstrument::SW01_temp = 0;
float xInstrument::SW01_press = 0;
float xInstrument::SW01_humidity = 0;
float xInstrument::SW10_temp = 0;
float xInstrument::SW12_temp = 0;
float xInstrument::SW12_humidity = 0;
float xInstrument::SL01_lux = 0;
float xInstrument::SL01_uva = 0;
float xInstrument::SL01_uvb = 0;

xInstrument::xInstrument()
{
}


// Enable CW01 LEDs
bool xInstrument::enableLEDS()
{
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
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
  return true;
}

// Enable Serial port @BR 115200
bool xInstrument::enableSerial()
{
  Serial.begin(115200);

  Serial_enabled = true;

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
    if (xCore.ping(BME280_I2C_ADDRESS))
    {
      SW01.begin();
      ACK_SW01 = 0xFF;
    } else {
      ACK_SW01 = 0x00;
      return false;
    }
  }
  
  // SL01 sensor
  if (sensor == String("sl01"))
  {
	  if(xCore.ping(VEML6075_I2C_ADDRESS) && xCore.ping(TSL4531_I2C_ADDRESS))
	  {
		SL01.begin();
		ACK_SL01 = 0xFF;
	  }else{
		ACK_SL01 = 0x00;
	  }
  }


  // SW10 sensor
  if (sensor == String("sw10"))
  {
    if (xCore.ping(LM75B_I2C_ADDR))
    {
      SW10.begin();
      ACK_SW10 = 0xFF;
    } else {
      ACK_SW10 = 0x00;
      return false;
    }
  }

  // SW12 sensor
  if (sensor == String("sw12"))
  {
    if (xCore.ping(SW12_I2C_ADDR))
    {
      SW12.begin();
      ACK_SW12 = 0xFF;
    } else {
      ACK_SW12 = 0x00;
      return false;
    }
  }

  return true;
}

bool xInstrument::enableWiFi()
{
  WiFi_enabled  = true;
}

bool xInstrument::enableBlynkBLE()
{
  BlynkBLE_enabled  = true;
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

void xInstrument::myTimerEvent()
{
  if(ACK_SW01)
  {
	Blynk.virtualWrite(1, SW01_temp);
	Blynk.virtualWrite(2, SW01_press);
	Blynk.virtualWrite(3, SW01_humidity);
  }
  
  if(ACK_SL01)
  {
	Blynk.virtualWrite(4, SL01_lux);
	Blynk.virtualWrite(5, SL01_uva);
	Blynk.virtualWrite(6, SL01_uvb);
  }
  
  if(ACK_SW10)
  {
	Blynk.virtualWrite(7, SW10_temp);
  }
  
  if(ACK_SW12)
  {
	Blynk.virtualWrite(8, SW12_temp);
	Blynk.virtualWrite(9, SW12_humidity);
  }
  
  
}

void xInstrument::sendData(String params) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client)
  {
    client -> setCACert(root_ca);
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + gas + "/exec?" + params;
	//Serial.println(url);
    if (http.begin(*client, url)); //Specify the URL and certificate
    int httpCode = http.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        //Serial.println(payload);

        http.end();
      }
    } else {
      //Serial.printf("[HTTPS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
   delete client;
  } else {
    //Serial.println("Unable to create client");
  }
}

bool xInstrument::begin()
{

  if (WiFi_enabled || BlynkBLE_enabled || Gsheet_enabled || AWS_enabled || Azure_enabled)
  {
    prv.begin();

    if (WiFi_enabled && !BlynkBLE_enabled)
    {
		
      //prv.nameJsonFile(uName);
      prv.addVariable("WiFi_SSID", "yourNetworkName");
      prv.addVariable("WiFi_PSK", "yourNetworkPassword");
    }
	
	if(Gsheet_enabled && !BlynkBLE_enabled)
	{
	  prv.addVariable("GAS_ID", "GoogleScriptID");
	}
	
	if(Azure_enabled && !BlynkBLE_enabled)
	{
	  prv.addVariable("Azure_string", "Azure String");
	}
	
	if(BlynkBLE_enabled)
	{
	  prv.addVariable("Blynk_AUTH", "Blynk BLE Authentication");
	}

	prv.transmit();
	//Writes this string, with the CRLF and SYNC prefixed and CRLF suffixed

	prv.receive();
	//Reads this string, with CRLF suffixed

    if (prv.success())
    {
      if (WiFi_enabled && !BlynkBLE_enabled)
      {
        prv.getVariable("WiFi_SSID", ssid);
        prv.getVariable("WiFi_PSK", password);
      }
	  
	  if(Gsheet_enabled && !BlynkBLE_enabled)
	  {
		prv.getVariable("GAS_ID",  gas);
	  }
	  
	  if(BlynkBLE_enabled)
	  {
		  prv.getVariable("Blynk_AUTH", Blynk_AUTH);
		  Blynk.setDeviceName("xInstrument");
		  Blynk.begin(Blynk_AUTH.c_str());
          timer.setInterval(5000L, myTimerEvent);
	  }

      if (Serial_enabled && WiFi_enabled)
      {
        Serial.println(ssid);
        Serial.println(password);
      }
	  
	  if (Serial_enabled && BlynkBLE_enabled)
      {
        Serial.println(Blynk_AUTH);
      }

      if (WiFi_enabled && !BlynkBLE_enabled)
      {
        WiFi.begin(ssid.c_str(), password.c_str());

        while (WiFi.status() != WL_CONNECTED) {
          digitalWrite(LED_RED, HIGH);
          delay(250);
          digitalWrite(LED_RED, LOW);
          delay(250);
        }
      }
	  
	  
	  if(Azure_enabled && !BlynkBLE_enabled)
	  {
		 prv.getVariable("Azure_string", connection_string);
		 Esp32MQTTClient_Init((const uint8_t*)connection_string.c_str(), true);
	  }

      if (Serial_enabled && !BlynkBLE_enabled)
      {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
      }

      if (WiFi_enabled && !BlynkBLE_enabled)
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

void xInstrument::loop()
{
	
  if(ACK_SW01)
  {
	  SW01.poll();
	  SW01_temp = SW01.getTempC();
	  SW01_press = SW01.getPressure();
	  SW01_humidity = SW01.getHumidity();
  }
  
  
  if(ACK_SW10)
  {
	  SW10.poll();
	  SW10_temp = SW10.getTempC();
  }
  
  if(ACK_SW12)
  {
	  SW12.getTempC(SW12_temp);
	  SW12.getHumidity(SW12_humidity);
  }
  
  if(ACK_SL01)
  {
	  SL01.poll();
	  SL01_lux = SL01.getLUX();
	  SL01_uva = SL01.getUVA();
	  SL01_uvb = SL01.getUVB();
  }
  
  if (Serial_enabled && !BlynkBLE_enabled)
  {
    if (ACK_SW01)
    {
      Serial.print(String(SW01_temp) + ",");
      Serial.print(String(SW01_press / 100.0) + ",");
      Serial.print(String(SW01_humidity) + ",");
    }
	
	if (ACK_SL01)
    {
      Serial.print(String(SL01_lux) + ",");
      Serial.print(String(SL01_uva) + ",");
      Serial.print(String(SL01_uvb) + ",");
    }
	
	
	if (ACK_SW12)
    {
      Serial.print(String(SW12_temp) + ",");
      Serial.print(String(SW12_humidity) + ",");
    }
	
	
	Serial.println();
  }
  
  if(BlynkBLE_enabled)
  {
	Blynk.run();
	timer.run(); // Initiates BlynkTimer
  }
	
  if (OD01_enabled && !BlynkBLE_enabled)
  {
    if (ACK_SW01)
    {
	  OD01.clear();
      OD01.println("SW01_Temp_" + String(SW01_temp) + "_C");
      OD01.println("SW01_Press_" + String(SW01_press / 100.0) + "_hPa");
      OD01.println("SW01_Hum_" + String(SW01_humidity) + "_%RH");
	  delay(2000);
    }
	
	if (ACK_SL01)
    {
	  OD01.clear();
      OD01.println("SL01_Lux_" + String(SL01_lux) + "_lux");
      OD01.println("SL01_UVA_" + String(SL01_uva) + "_uW/m^2");
      OD01.println("SL01_UVB_" + String(SL01_uvb) + "_uW/m^2");
	  delay(2000);
    }
	
    if (ACK_SW10)
    {
	  OD01.clear();
      OD01.println("SW10_Temp_" + String(SW10_temp) + "_C");
	  delay(2000);
    }
	
	if (ACK_SW12)
    {
	  OD01.clear();
      OD01.println("SW12_Temp_" + String(SW12_temp) + "_C");
	  OD01.println("SW12_Hum_" + String(SW12_humidity) + "_%RH");
	  delay(2000);
    }
  }

  
  // Send data to Gsheet
  if(Gsheet_enabled && WiFi_enabled && !BlynkBLE_enabled)
  {
	
	  digitalWrite(LED_GREEN, HIGH);
	  
	  gsheet_string = "timestamp=" + String(millis());
	  
	  if(ACK_SW01)
	  {
		  gsheet_string += "&sw01_temp=" + String(SW01_temp);
		  gsheet_string += "&sw01_press=" + String(SW01_press);
		  gsheet_string += "&sw01_hum=" + String(SW01_humidity);
	  }
	  
	  if(ACK_SL01)
	  {
		  gsheet_string += "&sl01_lux=" + String(SL01_lux);
		  gsheet_string += "&sl01_uva=" + String(SL01_uva);
		  gsheet_string += "&sl01_uvb=" + String(SL01_uvb);
	  }
		  
	  
	  if(ACK_SW10)
	  {
		  gsheet_string += "&sw10_temp=" + String(SW10_temp);
	  }
	  
	  if(ACK_SW12)
	  {
		  gsheet_string += "&sw12_temp=" + String(SW12_temp);
		  gsheet_string += "&sw12_humidity=" + String(SW12_humidity);
	  }
		  
		  // Send Data to gsheet
		  sendData(gsheet_string);
	}
	  
	  digitalWrite(LED_GREEN, LOW);
	  
	  delay(2000);
  
    // Send data to Azure
  if(Azure_enabled && WiFi_enabled && !BlynkBLE_enabled)
  {
	  
	  if(ACK_SW01)
	  {
		  root["sw01_temp"] = SW01_temp;
		  root["sw01_press"] = SW01_press;
		  root["sw01_hum"] = SW01_humidity;
	  }
	  
	  if(ACK_SL01)
	  {
		  root["sl01_lux"] = SL01_lux;
		  root["sl01_uva"] = SL01_uva;
		  root["sl01_uvb"] = SL01_uvb;
	  }
	  
	  
	  if(ACK_SW10)
	  {
		  root["sw10_temp"] = SW10_temp;
	  }
	  
	  if(ACK_SW12)
	  {
		  root["sw12_temp"] = SW12_temp;
		  root["sw12_hum"] = SW12_humidity;
	  }
	azure_string = "";
	#if ARDUINOJSON_VERSION_MAJOR == 5
		root.printTo(azure_string);
	#elif ARDUINOJSON_VERSION_MAJOR == 6
		serializeJson(root, azure_string);
	#endif

	
	EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(azure_string.c_str(), MESSAGE);
    Esp32MQTTClient_Event_AddProp(message, "CW02_instrument", "true");
    Esp32MQTTClient_SendEventInstance(message);
	
	delay(2000);
  }
  
}
