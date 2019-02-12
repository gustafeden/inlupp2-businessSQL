#pragma once
#include <Servo.h>

class ServoControl {
private:
	Servo myServo;
	int hatchPos;
public:
	ServoControl(int servopin) {
		myServo.attach(servopin);
		myServo.write(0);
		hatchPos = 0;
	}
	void openHatch() {
		myServo.write(85);
		hatchPos = 1;
	}
	void closeHatch() {
		myServo.write(0);
		hatchPos = 0;
	}
	int getPos() {
		return hatchPos;
	}
	void toggleHatch() {
		hatchPos = hatchPos == 1 ? 0 : 1;
		myServo.write(hatchPos == 1 ? 85 : 0);
	}
};