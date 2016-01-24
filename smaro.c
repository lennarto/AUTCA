#include <stdio.h>
#include <io.h>
#include "head.h"

// SPEED DEFINIEREN
int slopespeed = 45;
int slopespeed_fast = 55;
int slopespeed_slow = 45;

// LOGIK FWD BWD
// V = 1  : VORWÄRTS
// V = -1 : RÜCKWÄRTS
// r = -1 : FLAG : noch nicht gewendet
int v = 1;
int r = -1;

int main() {
	// VARIABLEN INITIALISIEREN
	printf("START\n!!!!!!!!!!!!!!!!!!!!!!!!\n");
	init();
	short AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ, x, y, z;
	char cBuff[32];
	
	// LICHT
	//volatile unsigned int *pLights = (volatile unsigned int*)(0x80000000 | LIGHTING_0_BASE);
	//*pLights = 0xf;
	
	*pwm_enable = 0;

	I2CWrite(MPU_SLAVE_ADDRESS, 0x6b, 0x0);
	I2CRead(MPU_SLAVE_ADDRESS, 0x75, 1, cBuff);

	while(1)
	{
		// TODO Wir brauchen nur AcX und AcY
		AcX = AcY = AcZ = Tmp = GyX = GyY = GyZ = 0;
		I2CRead(MPU_SLAVE_ADDRESS, 0x3B, 14, cBuff);
		AcX = (cBuff[0] << 8) | (cBuff[1] & 0xff);
		AcY = (cBuff[2] << 8) | (cBuff[3] & 0xff);
		AcZ = cBuff[4] << 8 | (cBuff[5] & 0xff);
		Tmp = (cBuff[6] << 8) | (cBuff[7] & 0xff);
		GyX = (cBuff[8] << 8) | (cBuff[9] & 0xff);
		GyY = (cBuff[10] << 8) | (cBuff[11] & 0xff);
		GyZ = (cBuff[12] << 8) | (cBuff[13] & 0xff);

		printf("AcX = %d\n", AcX);
		printf("AcY = %d/n", AcY);

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

		// ERHÖHTE STEIGUNG
		// RÜCKWÄRTS HOCH FAHREN
		if (AcY < -3500*v && r < 0) {
			wenden();
			v = -1;
			r = 1;
			slopespeed = slopespeed_fast;
			slow(ALL_WHEEL_BWD_MASK);
		}

		// ZU HOHE STEIGUNG
		// RUTSCHE RUNTER FAHREN
		if (AcY < -3500*v) {
			*pwm_enable = 0;
			v = 1;
			slopespeed = slopespeed_slow;
			slow(ALL_WHEEL_FWD_MASK);
		}
		
		// AUTO ZU WEIT LINKS
		if (AcX < -1200*v) {
			printf("nach rechts");
			nach_rechts();
			if(v > 0){
				slow(ALL_WHEEL_FWD_MASK);
			} else {
				slow(ALL_WHEEL_BWD_MASK);
			}
		}
			
		// AUTO ZU WEIT RECHTS
		if (AcX > 800*v) {
			printf("nach links");
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
	delay(1000000);
	*pwm_enable = 0;
}

void wenden() {
	*pwm_enable = 0;
	delay(2);
	*pwm_enable = (TURN_LEFT_MASK);
	delay(1000000);
	*pwm_enable = (TURN_LEFT_MASK);
	delay(1000000);
	*pwm_enable = 0;
}

void nach_links() {
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
		set_duty_cycle(pRearRightDutySet, 0);
		set_duty_cycle(pRearLeftDutySet, 0);
		set_duty_cycle(pFrontLeftDutySet, slopespeed * 2);
		*pwm_enable = (FRONT_LEFT_WHEEL_FWD_MASK);
	}
	delay(2000000);
	*pwm_enable = 0;
}

void nach_rechts() {
	*pwm_enable = 0;
	delay(2);
	if(v > 0){
		set_duty_cycle(pFrontRightDutySet, 0);
		set_duty_cycle(pRearRightDutySet, 0);
		set_duty_cycle(pRearLeftDutySet, slopespeed * 2);
		set_duty_cycle(pFrontLeftDutySet, 0);
		*pwm_enable = (REAR_LEFT_WHEEL_FWD_MASK);
	} else {
		set_duty_cycle(pFrontRightDutySet, slopespeed * 2);
		set_duty_cycle(pRearRightDutySet, 0);
		set_duty_cycle(pRearLeftDutySet, 0);
		set_duty_cycle(pFrontLeftDutySet, 0);
		*pwm_enable = (FRONT_RIGHT_WHEEL_FWD_MASK);
	}
	delay(2000000);
	*pwm_enable = 0;
}

