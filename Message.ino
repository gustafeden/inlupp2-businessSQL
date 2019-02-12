
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

void createMessage(char *payload, ServoControl & ventiHatch, DHT11sensor & tempdevice)
{
	float temperature = tempdevice.readTemp();
	float humidity = tempdevice.readHumid();

	StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["deviceId"] = DEVICE_ID;
	root["dateTime"] = (String)time(NULL);
	root["deviceType"] = DEVICE_TYPE;
	root["location"] = LOCATION;
	root["longitude"] = LONGITUDE;
	root["latitude"] = LATITUDE;


	if (std::isnan(temperature))
	{
		root["temperature"] = NULL;
	}
	else
	{
		root["temperature"] = temperature;
	}

	if (std::isnan(humidity))
	{
		root["humidity"] = NULL;
	}
	else
	{
		root["humidity"] = humidity;
	}

	root["hatchPos"] = ventiHatch.getPos();
	root.printTo(payload, MESSAGE_MAX_LEN);
}

void parseTwinMessage(char *message)
{
	StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(message);
	Serial.print("Twin device message: ");
	Serial.println(message);
	if (!root.success())
	{
		Serial.printf("Parse %s failed.\r\n", message);
		return;
	}

	if (root["desired"]["send_interval"].success())
	{
		send_interval = root["desired"]["send_interval"];
	}
	else if (root.containsKey("send_interval"))
	{
		send_interval = root["send_interval"];
	}

	if (root["desired"]["check_interval"].success())
	{
		check_interval = root["desired"]["check_interval"];
	}
	else if (root.containsKey("check_interval"))
	{
		check_interval = root["check_interval"];
	}
}