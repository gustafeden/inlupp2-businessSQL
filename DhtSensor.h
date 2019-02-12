#pragma once
#include <DHT.h>
class DHT11sensor {
private:
	DHT *dht;
public:
	DHT11sensor(int dhtpin, int dhttype) {
		dht = new DHT(dhtpin, dhttype);
		initDHT11();
	}
	~DHT11sensor() {
		delete dht;
	}
	void initDHT11() {
		dht->begin();
	}
	float readTemp() {
		return dht->readTemperature();
	}
	float readHumid() {
		return dht->readHumidity();
	}
};