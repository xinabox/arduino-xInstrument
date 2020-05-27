/*
* cw02_instrument.h - Library for CW02 instrument core
* Created by KalbeAbbas @XinaBox 05/05/2020
*/
#ifndef xINSTRUMENT
#define xINSTRUMENT

#define BLYNK_USE_DIRECT_CONNECT

//WiFi functionality
#include "Arduino.h"
#include "xCore.h"
#include "xOD01.h"
#include "xProvision.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "AzureIotHub.h"
#include "Esp32MQTTClient.h"
#include "ArduinoJson.h"

// BLE libs
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>


#define OD01_I2C_ADDRESS 				0x3C

#define LED_RED 25
#define LED_GREEN 26
#define LED_BLUE 27

#define DEVICE_ID "xInstrument"

class xInstrument
{
	public:
		xInstrument(void);
		bool enableOD01(void);
		bool enableSerial(void);
		bool enableSensor(String sensor_name);
		bool enableLEDS(void);
		void loop();
		bool begin(void);
		bool enableWiFi(void);
		bool enableBlynkBLE(void);
		bool enableGoogleSheet(void);
		bool enableAzure(void);
		bool enableAWS(void);
		bool enableSPIFFS(void);
		static void myTimerEvent(void);
		void sendData(String params);
		void updateAzure(char* var, float value);
		void updateAzure(String str);
		void createVariables(int num, ...);
		void updateVariables(int num, ...);
	
	private:
		xOD01 OD01;
		xProvision prv;
		static uint8_t num_vars;
		String ssid, password;
		String gas;
		String Blynk_AUTH;
		boolean WiFi_enabled;
		boolean BlynkBLE_enabled;
		boolean Gsheet_enabled;
		boolean Azure_enabled;
		boolean AWS_enabled;
		boolean SPIFFS_enabled;
		boolean Serial_enabled;
		boolean OD01_enabled;
		String gsheet_string;
		String connection_string;
		String azure_string;
		BlynkTimer timer;
		#if ARDUINOJSON_VERSION_MAJOR == 5
			DynamicJsonBuffer jsonBuffer;
			JsonObject& root = jsonBuffer.createObject();
		#elif ARDUINOJSON_VERSION_MAJOR == 6
			DynamicJsonDocument root(1024);
		#endif
		static char* vars[50];
		static double values[50];
		PROGMEM const char * root_ca=\
                                "-----BEGIN CERTIFICATE-----\n"\
                                "MIIESjCCAzKgAwIBAgINAeO0mqGNiqmBJWlQuDANBgkqhkiG9w0BAQsFADBMMSAw\n"\
                                "HgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFs\n"\
                                "U2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEy\n"\
                                "MTUwMDAwNDJaMEIxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3Qg\n"\
                                "U2VydmljZXMxEzARBgNVBAMTCkdUUyBDQSAxTzEwggEiMA0GCSqGSIb3DQEBAQUA\n"\
                                "A4IBDwAwggEKAoIBAQDQGM9F1IvN05zkQO9+tN1pIRvJzzyOTHW5DzEZhD2ePCnv\n"\
                                "UA0Qk28FgICfKqC9EksC4T2fWBYk/jCfC3R3VZMdS/dN4ZKCEPZRrAzDsiKUDzRr\n"\
                                "mBBJ5wudgzndIMYcLe/RGGFl5yODIKgjEv/SJH/UL+dEaltN11BmsK+eQmMF++Ac\n"\
                                "xGNhr59qM/9il71I2dN8FGfcddwuaej4bXhp0LcQBbjxMcI7JP0aM3T4I+DsaxmK\n"\
                                "FsbjzaTNC9uzpFlgOIg7rR25xoynUxv8vNmkq7zdPGHXkxWY7oG9j+JkRyBABk7X\n"\
                                "rJfoucBZEqFJJSPk7XA0LKW0Y3z5oz2D0c1tJKwHAgMBAAGjggEzMIIBLzAOBgNV\n"\
                                "HQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1Ud\n"\
                                "EwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFJjR+G4Q68+b7GCfGJAboOt9Cf0rMB8G\n"\
                                "A1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd/cGYYuMDUGCCsGAQUFBwEBBCkwJzAl\n"\
                                "BggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdvb2cvZ3NyMjAyBgNVHR8EKzAp\n"\
                                "MCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dzcjIvZ3NyMi5jcmwwPwYDVR0g\n"\
                                "BDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly9wa2kuZ29vZy9y\n"\
                                "ZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEAGoA+Nnn78y6pRjd9XlQWNa7H\n"\
                                "TgiZ/r3RNGkmUmYHPQq6Scti9PEajvwRT2iWTHQr02fesqOqBY2ETUwgZQ+lltoN\n"\
                                "FvhsO9tvBCOIazpswWC9aJ9xju4tWDQH8NVU6YZZ/XteDSGU9YzJqPjY8q3MDxrz\n"\
                                "mqepBCf5o8mw/wJ4a2G6xzUr6Fb6T8McDO22PLRL6u3M4Tzs3A2M1j6bykJYi8wW\n"\
                                "IRdAvKLWZu/axBVbzYmqmwkm5zLSDW5nIAJbELCQCZwMH56t2Dvqofxs6BBcCFIZ\n"\
                                "USpxu6x6td0V7SvJCCosirSmIatj/9dSSVDQibet8q/7UK4v4ZUN80atnZz1yg==\n"\
                                "-----END CERTIFICATE-----\n";
		
};


#endif
