#include <stdio.h>
#include <io.h>
#include "head.h"

// SPEED DEFINIEREN
int slopespeed = 45;
int slopespeed_fast = 55;
int slopespeed_slow = 30;
int wenden_speed = 45;
int delay_n = 10000000/3;

// TURN AROUND DELAY
int delayXXX;
int delay_kurz = 10000000*0.60;
int delay_lang = 10000000*0.75;

// LIGHTS
volatile unsigned int *pLights = (volatile unsigned int*)(0x80000000 | LIGHTING_0_BASE);

// LOGIK FWD BWD
// V = 1  : VORW�RTS
// V = -1 : R�CKW�RTS
// r = -1 : FLAG : noch nicht gewendet
int v = 1;
int r = -1;

int main() {
	// VARIABLEN INITIALISIEREN
	printf("START\n!!!!!!!!!!!!!!!!!!!!!!!!\n");
	int a = 0;
	init();
	short AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
	char cBuff[32];
	int wenden_speed;

	// LICHT
	*pLights = 0xf;
	*pwm_enable = 0;

	I2CWrite(MPU_SLAVE_ADDRESS, 0x6b, 0x0);
	I2CRead(MPU_SLAVE_ADDRESS, 0x75, 1, cBuff);

	while(1)
	{
		// Wir brauchen nur AcX und AcY
		AcX = AcY = AcZ = Tmp = GyX = GyY = GyZ = 0;
		I2CRead(MPU_SLAVE_ADDRESS, 0x3B, 14, cBuff);
		AcX = (cBuff[0] << 8) | (cBuff[1] & 0xff);
		AcY = (cBuff[2] << 8) | (cBuff[3] & 0xff);
		AcZ = cBuff[4] << 8 | (cBuff[5] & 0xff);
		Tmp = (cBuff[6] << 8) | (cBuff[7] & 0xff);
		GyX = (cBuff[8] << 8) | (cBuff[9] & 0xff);
		GyY = (cBuff[10] << 8) | (cBuff[11] & 0xff);
		GyZ = (cBuff[12] << 8) | (cBuff[13] & 0xff);

		printf("AcX = %d - ", AcX);
		printf("AcY = %d\n", AcY);

		// WERTE AUF STANDARD SETZTEN
		// UND AUTO GERADEAUS FAHREN LASSEN
		set_duty_cycle(pFrontRightDutySet, slopespeed);
		set_duty_cycle(pRearRightDutySet, slopespeed);
		set_duty_cycle(pRearLeftDutySet, slopespeed);
		set_duty_cycle(pFrontLeftDutySet, slopespeed);

		// LOGIK FWD BWD
		if(v > 0){
			slow(ALL_WHEEL_FWD_MASK);
		} else {
			slow(ALL_WHEEL_BWD_MASK);
		}

		printf("v = %d\n", v);

		// TURN AROUND
		if (AcY < -3600*v && r < 0 && AcX > -800 && AcX < 800) {
			*pLights = 0xf;
			delay(1000000);

			if (AcX > 200 && AcX < 300) {
				delayXXX = 10000000*0.70;
				printf("delay kurz 075");
				a = 0;
		  	}

			if (AcX > 301 && AcX < 400) {
				delayXXX = 10000000*0.65;
				printf("delay kurz 07");
				a = 0;
			}

			if (AcX > 401 && AcX < 500) {
				delayXXX = 10000000*0.55;
				printf("delay kurz 06");
			}

			if (AcX > 501 && AcX < 600) {
				delayXXX = 10000000*0.5;
				printf("delay 05");
				a = 0;
			}

			if (AcX < 200 && AcX > -200) {
				delayXXX = 10000000*0.55;
				printf("delay 07");
				a = 0;
		  	}

			if (AcX < -201 && AcX > -300) {
				delayXXX = 10000000*0.75;
				printf("delay 075");
				a = 1;
			}

			if (AcX < -301 && AcX > -400) {
				delayXXX = 10000000*0.8;
				printf("delay 08");
				a = 1;
			}

			if (AcX < -401 && AcX > -500) {
			  	delayXXX = 10000000*0.85;
				printf("delay 058");
				a = 1;
			}

			if (AcX < -501 && AcX > -600) {
				delayXXX = 10000000*0.9;
				printf("delay 099");
		   		a = 1;
			}

			else {
		    		delayXXX = 10000000*0.60;
				printf("delay lang");
			}

			// TURN AROUND WITH DALAY a
			wenden(a);

			pos_halten();
			printf("!!! WENDEN !!!\n");

			// v = -1;
			r = 1;
			slopespeed = slopespeed_slow;
			*pLights = 0x1;
			slow(ALL_WHEEL_FWD_MASK);
			printf("v = %i\n", v);
		}

		if (AcY < -2000) {
			delay_n = 1000000;
		}

		delay_n = 1000000;

		// AUTO ZU WEIT LINKS
		if (AcX < -1000*v) {
			printf("nach rechts\n");
			nach_rechts();

			if(v > 0){
				slow(ALL_WHEEL_FWD_MASK);
			} else {
				slow(ALL_WHEEL_BWD_MASK);
			}
		}

		// AUTO ZU WEIT RECHTS
		if (AcX > 400*v) {
			printf("nach links\n");
			nach_links();

			if(v > 0){
				slow(ALL_WHEEL_FWD_MASK);
			} else {
				slow(ALL_WHEEL_BWD_MASK);
			}
		}
		
		delay(10000000);
	}

	return 0;
}

void slow(RICHTUNG) {
	*pwm_enable = 0;
	delay(2);
	*pwm_enable = (RICHTUNG);
	delay(delay_n*2);
	*pwm_enable = 0;
}

void wenden(int a) {
	*pwm_enable = 0;
	delay(2);
	set_duty_cycle(pFrontRightDutySet, wenden_speed);
	set_duty_cycle(pRearRightDutySet, wenden_speed);
	set_duty_cycle(pRearLeftDutySet, wenden_speed);
	set_duty_cycle(pFrontLeftDutySet, wenden_speed);
	if(a==0) *pwm_enable = (TURN_LEFT_MASK);
	if(a == 1) *pwm_enable = (TURN_RIGHT_MASK);
	delay(delayXXX);

	set_duty_cycle(pFrontRightDutySet, slopespeed);
	set_duty_cycle(pRearRightDutySet, slopespeed);
	set_duty_cycle(pRearLeftDutySet, slopespeed);
	set_duty_cycle(pFrontLeftDutySet, slopespeed);
}

void pos_halten() {
	*pwm_enable = 0;
	delay(2);
	set_duty_cycle(pFrontRightDutySet, 20);
	set_duty_cycle(pRearRightDutySet, 20);
	set_duty_cycle(pRearLeftDutySet, 20);
	set_duty_cycle(pFrontLeftDutySet, 20);
	*pwm_enable = (ALL_WHEEL_BWD_MASK);
	delay(delayXXX*3);

	set_duty_cycle(pFrontRightDutySet, slopespeed);
	set_duty_cycle(pRearRightDutySet, slopespeed);
	set_duty_cycle(pRearLeftDutySet, slopespeed);
	set_duty_cycle(pFrontLeftDutySet, slopespeed);
}

void nach_links() {
	*pLights = 0x8;
	*pwm_enable = 0;
	delay(2);
	if(v > 0){
		set_duty_cycle(pFrontRightDutySet, 0);
		set_duty_cycle(pRearRightDutySet, slopespeed * 2);
		set_duty_cycle(pRearLeftDutySet, 0);
		set_duty_cycle(pFrontLeftDutySet, 0);
		*pwm_enable = (REAR_RIGHT_WHEEL_FWD_MASK);
	} else {
		set_duty_cycle(pFrontRightDutySet, 0);
		set_duty_cycle(pRearRightDutySet, slopespeed * 2);
		set_duty_cycle(pRearLeftDutySet, 0);
		set_duty_cycle(pFrontLeftDutySet, 0);
		*pwm_enable = (REAR_RIGHT_WHEEL_BWD_MASK);
	}
	delay(2000000);
	*pwm_enable = 0;
	*pLights = 0x1;
}

void nach_rechts() {
	*pwm_enable = 0x3;
	delay(2);
	if(v > 0){
		set_duty_cycle(pFrontRightDutySet, 0);
		set_duty_cycle(pRearRightDutySet, 0);
		set_duty_cycle(pRearLeftDutySet, slopespeed * 2);
		set_duty_cycle(pFrontLeftDutySet, 0);
		*pwm_enable = (REAR_LEFT_WHEEL_FWD_MASK);
	} else {
		set_duty_cycle(pFrontRightDutySet, 0);
		set_duty_cycle(pRearRightDutySet, 0);
		set_duty_cycle(pRearLeftDutySet, slopespeed * 2);
		set_duty_cycle(pFrontLeftDutySet, 0);
		*pwm_enable = (REAR_LEFT_WHEEL_BWD_MASK);
	}
	delay(2000000);
	*pwm_enable = 0;
	*pLights = 0x1;
}
