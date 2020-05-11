/*
* cw02_instrument.h - Library for CW02 instrument core
* Created by KalbeAbbas @XinaBox 05/05/2020
*/

#ifndef CW02_INSTRUMENT
#define CW02_INSTRUMENT

#include "Arduino.h"
#include "xCore.h"
#include "xOD01.h"
#include "xProvision.h"
#include "WiFi.h"
#include "HTTPClient.h"

// All Weather sensors
#include "xSW01.h"
#include "xSW03.h"
#include "xSW10.h"
#include "xSW12.h"

#define OD01_I2C_ADDRESS 				0x3C
#define BME280_I2C_ADDRESS 				0x76
#define MPL3115A2_I2C_ADDRESS			0x60
#define LM75B_I2C_ADDR 				    0x48
#define SW12_I2C_ADDR 				    0x5F

#define LED_RED 25
#define LED_GREEN 26
#define LED_BLUE 27

class xInstrument
{
	public:
		xInstrument(void);
		bool enableOD01(void);
		bool enableSerial(void);
		bool enableSensor(String sensor_name);
		void loop();
		bool begin(void);
		bool enableWiFi(void);
		bool enableBLE(void);
		bool enableGoogleSheet(void);
		bool enableAzure(void);
		bool enableAWS(void);
		bool enableSPIFFS(void);
		void sendData(String params);
	
	private:
		xOD01 OD01;
		xSW01 SW01;
		xSW03 SW03;
		xSW10 SW10;
		xSW12 SW12;
		xProvision prv;
		String ssid, password;
		String gas;
		boolean ACK_SW01;
		boolean ACK_SW03;
		boolean ACK_SW10;
		boolean ACK_SW12;
		boolean WiFi_enabled;
		boolean BLE_enabled;
		boolean Gsheet_enabled;
		boolean Azure_enabled;
		boolean AWS_enabled;
		boolean SPIFFS_enabled;
		boolean Serial_enabled;
		boolean OD01_enabled;
		float SW01_temp;
		float SW01_press;
		float SW01_humidity;
		String gsheet_string;
	PROGMEM const char * root_ca=\
	"-----BEGIN CERTIFICATE-----\n" \
	"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
	"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
	"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
	"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
	"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
	"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
	"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
	"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
	"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
	"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
	"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
	"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
	"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
	"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
	"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
	"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
	"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
	"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
	"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
	"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
	"-----END CERTIFICATE-----\n";
		
};

#endif