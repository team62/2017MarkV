#pragma config(Sensor, in1,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  liftStop,       sensorTouch)
#pragma config(Sensor, dgtl2,  claw,           sensorDigitalOut)
#pragma config(Sensor, dgtl9,  leftEnc,        sensorQuadEncoder)
#pragma config(Sensor, dgtl11, rightEnc,       sensorQuadEncoder)
#pragma config(Motor,  port1,           rightWheel1,   tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           rightWheel2,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           leftWheel1,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           leftLift1,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           rightLift1,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           rightLift2,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           rightLift3,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           leftLift2,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           leftLift3,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          leftWheel2,    tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JonLib2/JonLib2.h"

/*///////////////////////////////////////////////////////////
/////____________/\\\\\____/\\\\\\\\\_____              /////
///// ________/\\\\////___/\\\///////\\\___             /////
/////  _____/\\\///_______\///______\//\\\__            /////
/////   ___/\\\\\\\\\\\______________/\\\/___           /////
/////    __/\\\\///////\\\_________/\\\//_____          /////
/////     _\/\\\______\//\\\_____/\\\//________         /////
/////      _\//\\\______/\\\____/\\\/___________        /////
/////       __\///\\\\\\\\\/____/\\\\\\\\\\\\\\\_       /////
/////        ____\/////////_____\///////////////__      /////
///// 2017 Mark V Robot                                 /////
///// Main Code                                         /////
///// Authors: Jonathan Damico (jj_damico@yahoo.com)    /////
///// Since: March 24, 2017                             /////
*////////////////////////////////////////////////////////////

//required by JonLib2
//sets speed of left wheels on robot
void setLeftWheelSpeed (int speed) {
	motor[leftWheel1] = speed;
	motor[leftWheel2] = speed;
}

//required by JonLib2
//sets speed of right wheels on robot
void setRightWheelSpeed (int speed) {
	motor[rightWheel1] = speed;
	motor[rightWheel2] = speed;
}

//required by JonLib2
//takes number of inches for the robot to travel
//returns encoder ticks for right drivebase to reach that distance
int rightEncoderCurve (int target) {
	return 27.2*target;
}

//required by JonLib2
//takes number of inches for robot to travel
//returns encoder ticks for left drivebase to reach that distance
int leftEncoderCurve (int target) {
	return 27.6*target;
}

//required by JonLib2
//takes degrees for robot to travel
//returns gyro value for degrees
//90 degrees is a right turn
int gyroCurve (int target) {
	return -10.5*target;
}

//sets all lift motors to power
void lift (int power) {
	motor[leftLift1]  = power;
	motor[rightLift1] = power;
	motor[leftLift2]  = power;
	motor[rightLift2] = power;
	motor[leftLift3]  = power;
	motor[rightLift3] = power;
}

//task to test all motors with differient button inputs
task motorPortTest() {
	while(true) {
		motor[port1] 	= vexRT(Btn7U)*127; //rightDrive1 R
		motor[port2] 	= vexRT(Btn7D)*127; //rightDrive2 R
		motor[port3] 	= vexRT(Btn7L)*127; //leftDrive1
		motor[port4] 	= vexRT(Btn7R)*127; //leftLift1
		motor[port5] 	= vexRT(Btn8U)*127; //rightLift1 R
		motor[port6] 	= vexRT(Btn8D)*127; //rightLift2
		motor[port7] 	= vexRT(Btn8L)*127; //rightLift3 R
		motor[port8] 	= vexRT(Btn8R)*127; //leftLift2 R
		motor[port9] 	= vexRT(Btn5U)*127; //leftLift3
		motor[port10]	= vexRT(Btn6U)*127; //leftDrive2
	}
}

void pre_auton() { }


#define LIFT_RAISE 2100
#define LIFT_DUMP 2900
#define LIFT_SLOW 2200
#define LIFT_DOWN -1

int liftTarget = 0;
int liftDelay = 0;
task autoMoveLift () {
	liftTarget = SensorValue[liftPot];
	while(true) {
		if(liftTarget == -1) {
			delay(liftDelay);
			liftDelay = 0;
			while(!SensorValue[liftStop]) {
				lift(limit(-127,SensorValue[liftPot]>LIFT_SLOW?127:60));
			}
			liftTarget = SensorValue[liftPot]-75;
		} else if (liftTarget == LIFT_DUMP) {
			delay(liftDelay);
			liftDelay = 0;
			lift(127);
			while(SensorValue[liftPot]<LIFT_DUMP-300) { delay(5); }
			lift(0);
			delay(500);
			liftTarget = SensorValue[liftPot];
		} else {
			lift(limit((liftTarget-SensorValue[liftPot])*0.25,SensorValue[liftPot]>LIFT_SLOW?127:60));
		}
	}
}

#define LIFT_PIPE_UP 3261
#define LIFT_PIPE_DOWN 3298
int liftInchesTarget = -1;
bool runLiftInches = false;
task liftInches() {
	while(true) {
		if(liftInchesTarget!=-1 && runLiftInches) {
			if(SensorValue[liftPot]<2900)
				lift(40);
			if(SensorValue[liftPot]<liftInchesTarget-(40/0.7))
				lift(20);
			else if(SensorValue[liftPot]>liftInchesTarget+(60/0.7))
				lift(-60);
			else if(liftInchesTarget>SensorValue[liftPot])
				lift(0.5*(liftInchesTarget-SensorValue[liftPot]));
			else
				lift(0.7*(liftInchesTarget-SensorValue[liftPot]));
		}
		delay(25);
	}
}

void close () {
	SensorValue[claw] = 0;
}

void open () {
	SensorValue[claw] = 1;
}

void setLiftTarget (int target) {
	liftTarget = target;
}

void setLiftTarget (int target, int delayTime) {
	liftDelay = delayTime;
	setLiftTarget(target);
}

void autonomousTest () {
	drivebase db;
	drivebase load;
	gyroscope g;
	clearDebugStream();
	startTask(autoMoveLift);
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	resetSensor(gyro); //.17, .5
	setLiftTarget(LIFT_RAISE);
	initPIDGyroscope(g, gyro, 0.25, 0.0001, 0.7, 30, -1, 40);
	//addGyroTargetPIDAutoPointTurn(g, 90);
	setWheelSpeed(0);

	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
	initPIDDrivebase(load, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);

	//addDrivebaseTargetPIDAuto(db, 15, 20);

	int targets[5];
	targets[0] = 48;
	targets[1] = 0;
	targets[2] = 12;
	targets[3] = 36;
	targets[4] = 0;

	for(int i = 0; i<5; i++) {
		setDrivebaseTargetPIDAuto(load, targets[i]);
		delay(200);
	}
}

void programmingSkils () {
	startTask(autoMoveLift);

	//init everything, consider moving to pre-auton task so everything is  initialised  before skills starts,  extra second or so
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	resetSensor(gyro);

	clearDebugStream();

	gyroscope g;
	drivebase db;
	gyroscope gLoad;
	drivebase dbLoad
	initPIDGyroscope(g, gyro, 0.25, 0.0001, 1.5, 30, -1, 40);
	initPIDGyroscope(gLoad, gyro, 0.25, 0.0001, 2.5, 30, -1, 40);
	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
	initPIDDrivebase(dbLoad, leftEnc, rightEnc, gyro, 0.8, 0.0001, 0.7, 30, -1, 20, 0.3);

	open();

	addDrivebaseTargetPIDAuto(db, 60);
	close();
	delay(500);
	setLiftTarget(LIFT_RAISE);
	setDrivebaseTargetPIDAuto(db, 12,11);

	setGyroTargetPIDAutoRightSwingTurn(gLoad, 90);
	delay(300);
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	setLiftTarget(LIFT_DUMP,1000);
	addDrivebaseTargetPIDAuto(dbLoad,  -47);
	setWheelSpeed(-30);
	while(SensorValue[liftPot]<LIFT_DUMP-500) { delay(5); }
	open();
	delay(100);
	setLiftTarget(-1);
	while(!SensorValue[liftStop]) { delay(5); }
	for(int i = 0;  i<2; i++) {
		addDrivebaseTargetPIDAuto(db, 36);
		delay(200);
		close();
		addDrivebaseTargetPIDAuto(db,  -50);
		setWheelSpeed(-30);
		setLiftTarget(LIFT_DUMP);
		delay(700);
		setWheelSpeed(-30);
		while(SensorValue[liftPot]<LIFT_DUMP-500) { delay(5); }
		open();
		setLiftTarget(LIFT_DOWN);
		while(SensorValue[liftPot]>LIFT_SLOW) { delay(5); }
	}
}

task autonomous () {
	//startTask(autoMoveLift);
	//setLiftTarget(LIFT_DOWN);
	//autonomousTest();
	programmingSkils();
}

void usercontrolMain () {
	startTask(liftInches);
	while(true) {
		tankDrive(vexRT[Ch3], vexRT[Ch2], 15);

		if(vexRT[Btn8U]) {
			runLiftInches = false;
			lift(-127);
		} else if(vexRT[Btn5U]) {
			runLiftInches  = false;
			lift(127);
		} else if(vexRT[Btn5D] && !SensorValue[liftStop]) {
			runLiftInches  = false;
			if(SensorValue[liftPot]>3100)
				lift(-127);
			else if(SensorValue[liftPot]<LIFT_SLOW)
				lift(-40);
			else
				lift(-80);
		}
		else if(SensorValue[liftStop] && ((abs(vexRT[Ch3])+abs(vexRT[Ch2])>15))) { //hold down code
			lift(-15);
			runLiftInches = false;
		} else if(SensorValue[liftStop]) {
			lift(0);
			runLiftInches = false;
		}
		else if(SensorValue[liftPot]>3100 && !runLiftInches)
			lift(-15);
		else if(SensorValue[liftPot]<2200 && !runLiftInches)
			lift(15);
		else if(!runLiftInches){
				lift(0);
		}

		if(vexRT[Btn7U]) {
			liftInchesTarget = LIFT_PIPE_UP;
			runLiftInches = true;
			while(vexRT[Btn7U]) { delay(5); }
		}

		if(vexRT[Btn7D]) {
			liftInchesTarget = LIFT_PIPE_DOWN;
			runLiftInches = true;
			while(vexRT[Btn7D]) { delay(5); }
		}

		if(vexRT[Btn6U]) {
			SensorValue[claw] = !SensorValue[claw];
			while(vexRT[Btn6U]) { delay(5); }
		}
	delay(25);
	}
}

task usercontrol() {
	//startTask(motorPortTest);
	usercontrolMain();
}
