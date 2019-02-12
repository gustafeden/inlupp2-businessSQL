/*
    Name:       inlupp2_businessSQL.ino
    Created:	2019-02-05 14:46:56
    Author:     LAPTOP-FTI3RT4H\gusta
*/

/*
skicka ett meddelande var 10 min
kolla temperaturen regelbundet
ifall temp går under 20 grader stäng ventilen och skicka meddelande
ifall temp går över 25 grader öppna ventilen och skicka meddelande
ifall man trycker på knappen ska ventilen öppnas/stängas och skicka meddelande
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>
//#include <Servo.h>


#include "ServoClass.h"
#include "DhtSensor.h"
#include "config.h"
static bool messagePending = false;
static bool messageSending = true;
static bool buttonPressed = false;
static bool sendMsg = false;
//static bool checkAlert = false;

static char *connectionString = "HostName=assignment-3-IoTHub.azure-devices.net;DeviceId=VentilationRegulator1;SharedAccessKey=PN0BOjQGBDGvkR3xV/ndTUfm9qsO/CAUJ5H7OFZxyng=";


static int send_interval = SEND_INTERVAL;
static int check_interval = CHECK_INTERVAL;

unsigned long currentMillis;
unsigned long lastSentMillis;
unsigned long lastCheckedMillis;
unsigned long lastButtonCheckMillis;
unsigned long alertSendTimer;
unsigned long lastHatchSwitch;

ESP8266WiFiMulti wifiMulti;
static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
static ServoControl ventilationHatch(SERVO_PIN);
static DHT11sensor dhtSensor(DHT_PIN, DHT_TYPE);

void blinkLED()
{
	digitalWrite(LED_PIN, HIGH);
	delay(500);
	digitalWrite(LED_PIN, LOW);
}

void initSerial()
{
	Serial.begin(115200);
	Serial.println("Serial successfully initiated.");
}
void initMultiWifi() {

	wifiMulti.addAP("iPhone x", "hejhejhej");
	wifiMulti.addAP("Caesar's Palace 2.0", "kingofpop");
	wifiMulti.addAP("IoT", "IoT2018!");
	Serial.println("Connecting");
	while (wifiMulti.run() != WL_CONNECTED) {
		delay(250);
		Serial.print('.');
	}
	Serial.println("\r\n");
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID());
	Serial.print("IP address:\t");
	Serial.print(WiFi.localIP());
	Serial.println("\r\n");
}

void initTime()
{
	time_t epochTime;
	configTime(0, 0, "pool.ntp.org", "time.nist.gov");

	while (true)
	{
		epochTime = time(NULL);

		if (epochTime == 0)
		{
			Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
			delay(2000);
		}
		else
		{
			Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
			break;
		}
	}
	delay(500);
}

void initIoTClient() {
	iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
	if (iotHubClientHandle == NULL)
	{
		Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
		while (1);
	}

	IoTHubClient_LL_SetOption(iotHubClientHandle, "product_info", "FeatherHuzzah");
	IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
	IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
	IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);
}
void setup() {
	pinMode(LED_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);
	initSerial();
	//Serial.setTimeout(2000);
	initMultiWifi();
	initTime();
	initIoTClient();
}
void loop()
{
	currentMillis = millis();



	if (currentMillis - lastButtonCheckMillis > 30 && currentMillis - lastSentMillis > 1000) {
		lastButtonCheckMillis = currentMillis;
		if (digitalRead(BUTTON_PIN) == HIGH)
			buttonPressed = true;
	}
	if (!messagePending && buttonPressed && currentMillis - lastHatchSwitch > 5000) {
		lastHatchSwitch = currentMillis;
		buttonPressed = false;
		sendMsg = true;
		ventilationHatch.toggleHatch();
	}
	else if (!messagePending && messageSending && !sendMsg && currentMillis - lastSentMillis > send_interval)
	{
		sendMsg = true;
	}
	else if (!sendMsg && currentMillis - lastCheckedMillis > check_interval && currentMillis - lastSentMillis < send_interval) {
		lastCheckedMillis = currentMillis;
		//Serial.print("checking temps: ");
		float temptemp = dhtSensor.readTemp();
		Serial.println(temptemp);
		sendMsg = temptemp > TEMPERATURE_MAX || temptemp < TEMPERATURE_MIN ? true : false;
		if (sendMsg) {
			if (temptemp > TEMPERATURE_MAX) {
				if (ventilationHatch.getPos() != 1)
					ventilationHatch.openHatch();
			}	
			else {
				if (ventilationHatch.getPos() != 0)
					ventilationHatch.closeHatch();
			}
		}
	}

	if (!messagePending && messageSending && sendMsg && currentMillis - alertSendTimer > ALERT_INTERVAL) {
		alertSendTimer = currentMillis;
		sendMsg = false;
		lastSentMillis = currentMillis;
		char messagePayload[MESSAGE_MAX_LEN];
		createMessage(messagePayload, ventilationHatch, dhtSensor);
		sendMessage(iotHubClientHandle, messagePayload);
	}
	
	IoTHubClient_LL_DoWork(iotHubClientHandle);
	delay(10);

}