#include <stdio.h>
#include <io.h>
#include "head.h"

int slopespeed = 45;
int slopespeed2 = 55;

void slow(RICHTUNG) {
	*pwm_enable = 0;
	delay(2);
	*pwm_enable = (RICHTUNG);
	delay(1000000);
	*pwm_enable = 0;
}

void nach_links() {
	*pwm_enable = 0;
	delay(2);
	set_duty_cycle(pFrontRightDutySet, 0);
	set_duty_cycle(pRearRightDutySet, slopespeed * 2);
	set_duty_cycle(pRearLeftDutySet, 0);
	set_duty_cycle(pFrontLeftDutySet, 0);
	*pwm_enable = (REAR_RIGHT_WHEEL_FWD_MASK);
	delay(2000000);
	*pwm_enable = 0;
}

void nach_rechts() {
	*pwm_enable = 0;
	delay(2);
	set_duty_cycle(pFrontRightDutySet, 0);
	set_duty_cycle(pRearLeftDutySet, slopespeed * 2);
	set_duty_cycle(pRearRightDutySet, 0);
	set_duty_cycle(pFrontLeftDutySet, 0);
	*pwm_enable = (REAR_LEFT_WHEEL_FWD_MASK);
	delay(2000000);
	*pwm_enable = 0;
}

int main() {
	printf("TEST!!!!!!!!!!!!!!!!!!!!!!!!\n");
	init();
	unsigned int i = 0;
	short AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ, x, y, z;
	char cBuff[32];
	//int p = 1;
	//volatile unsigned int *pLights = (volatile unsigned int*)(0x80000000 | LIGHTING_0_BASE);
	*pwm_enable = 0;
	//*pLights = 0xf;

	set_duty_cycle(pFrontRightDutySet, slopespeed);
	set_duty_cycle(pRearRightDutySet, slopespeed);
	set_duty_cycle(pRearLeftDutySet, slopespeed);
	set_duty_cycle(pFrontLeftDutySet, slopespeed);

	I2CWrite(MPU_SLAVE_ADDRESS, 0x6b, 0x0);
		I2CRead(MPU_SLAVE_ADDRESS, 0x75, 1, cBuff);
		printf("WhoAmI = %x\n", (unsigned int)cBuff[0]);

		while(1)
		{
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

			set_duty_cycle(pFrontRightDutySet, slopespeed);
			set_duty_cycle(pRearRightDutySet, slopespeed);
			set_duty_cycle(pRearLeftDutySet, slopespeed);
			set_duty_cycle(pFrontLeftDutySet, slopespeed);
			slow(ALL_WHEEL_FWD_MASK);

			 if (AcY < -3500) {
				slopespeed = slopespeed2;
			}

			if (AcX < -1200 ) {
				printf("nach rechts");
				nach_rechts();
				printf("test rechts");
				slow(ALL_WHEEL_FWD_MASK);
			}
			if (AcX > 800) {
				printf("nach links");
				nach_links();
				printf("test links");
				slow(ALL_WHEEL_FWD_MASK);
		    }
			delay(10000000);


		}
	*pwm_enable = 0;
	return 0;
}

