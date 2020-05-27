#include "Arduino.h"
#include "xInstrument.h"

char* xInstrument::vars[50] = {};
double xInstrument::values[50] = {};
uint8_t xInstrument::num_vars = 0;

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

void xInstrument::createVariables(int num, ...)
{
	num_vars = num;
	va_list valist;
	va_start(valist, num);
	
	for (int i = 0; i < num; i++) { //access all the arguments assigned to valist
      vars[i] = va_arg(valist, char*);
   }
   
   va_end(valist); //clean memory reserved for valist
}

void xInstrument::updateVariables(int num, ...)
{
	va_list valist;
	va_start(valist, num);
	
	for (int i = 0; i < num; i++) { //access all the arguments assigned to valist
      values[i] = va_arg(valist, double);
   }
   
   va_end(valist); //clean memory reserved for valist
}

void xInstrument::myTimerEvent()
{
	for(int i = 0; i < num_vars; i++)
	{
		Blynk.virtualWrite(i, values[i]);
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
          timer.setInterval(1000L, myTimerEvent);
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

void xInstrument::updateAzure(char* var, float value)
{
	if(Azure_enabled && WiFi_enabled && !BlynkBLE_enabled)
	{
		#if ARDUINOJSON_VERSION_MAJOR == 5
			DynamicJsonBuffer jsonBuffer;
			JsonObject& root = jsonBuffer.createObject();
		#elif ARDUINOJSON_VERSION_MAJOR == 6
			DynamicJsonDocument root(1024);
		#endif
		
		root[var] = value;
		
		azure_string = "";
		#if ARDUINOJSON_VERSION_MAJOR == 5
			root.printTo(azure_string);
		#elif ARDUINOJSON_VERSION_MAJOR == 6
			serializeJson(root, azure_string);
		#endif

		
		EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(azure_string.c_str(), MESSAGE);
		Esp32MQTTClient_Event_AddProp(message, "CW02_instrument", "true");
		Esp32MQTTClient_SendEventInstance(message);
	}
	
}

void xInstrument::updateAzure(String str)
{
	if(Azure_enabled && WiFi_enabled && !BlynkBLE_enabled)
	{
		
		azure_string =  str;
		
		EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(azure_string.c_str(), MESSAGE);
		Esp32MQTTClient_Event_AddProp(message, "CW02_instrument", "true");
		Esp32MQTTClient_SendEventInstance(message);
	
	}
}

void xInstrument::loop()
{
	
  
  if(BlynkBLE_enabled)
  {
	Blynk.run();
	timer.run(); // Initiates BlynkTimer
  }
	
  if (OD01_enabled && !BlynkBLE_enabled)
  {
	  OD01.home();
	  
	  for(int i = 0; i < num_vars; i++)
	  {
		  OD01.println(String(vars[i]) + "=" + String(values[i]));
	  }
  }

  
  // Send data to Gsheet
  if(Gsheet_enabled && WiFi_enabled && !BlynkBLE_enabled)
  {
	
	  digitalWrite(LED_GREEN, HIGH);
	  
	  gsheet_string = "timestamp=" + String(millis());
	  
	  for (int i = 0; i <  num_vars; i++)
	  {
		  gsheet_string += "&" + String(vars[i]) + "=" + String(values[i]);
	  }
	  
	  sendData(gsheet_string);
	  Serial.println(gsheet_string);
	  
	  delay(2000);
	  
	  digitalWrite(LED_GREEN, LOW);
	  
	  delay(2000);
	  
  }
  
  if(Serial_enabled)
  {
	  String print_str = "";
	  
	  for(int i = 0; i < num_vars; i++)
	  {
		  print_str += String(values[i]) + ",";
	  }
	  
	  Serial.println(print_str);
  }
  
    // Send data to Azure
  if(Azure_enabled && WiFi_enabled && !BlynkBLE_enabled)
  {
	
	for(int i = 0; i < num_vars; i++)
	{
		root[vars[i]] = values[i];
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
