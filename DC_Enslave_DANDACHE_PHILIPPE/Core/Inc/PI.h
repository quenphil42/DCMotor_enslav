/*
 * PI.h
 *
 *  Created on: Dec 8, 2022
 *      Author: quent
 */

#ifndef INC_PI_H_
#define INC_PI_H_


typedef struct {

	/* Controller gains */
	float Kp;
	float Ki;


	/* Output limits */
	float limMin_output;
	float limMax_output;

	/* Integrator limits */
	float limMin_integrator;
	float limMax_integrator;

	/* Sample time (in seconds) */
	float T;

	/* Controller "memory" */
	float integrator;
	float prevError;			/* Required for integrator */

	/* Controller output */
	float out;

} PIController;

void PIController_Init(PIController *pi);
float PIController_Update(PIController *pi, float consigne, float measurement);
float PIPrint_Out(PIController *pi);


#endif /* INC_PI_H_ */
