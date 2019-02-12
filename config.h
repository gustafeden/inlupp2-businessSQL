#pragma once
#define DEVICE_ID "VentilationRegulator1"
#define DEVICE_TYPE "VentilationRegulator"
#define DEVICE_MODEL "FeatherHuzzah"
#define DHT_TYPE DHT11

#define LOCATION "Home"
#define LONGITUDE "17.972290"
#define LATITUDE "59.382300"

#define LED_PIN 4
#define DHT_PIN 13
#define BUTTON_PIN 5
#define SERVO_PIN 12

#define TEMPERATURE_MAX 25
#define TEMPERATURE_MIN 20

#define SEND_INTERVAL 60000
#define ALERT_INTERVAL 60000
#define CHECK_INTERVAL 20000

#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256
