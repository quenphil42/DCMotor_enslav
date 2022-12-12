/*
 * PI.c
 *
 *  Created on: Dec 8, 2022
 *      Author: quent
 */


#include "PI.h"

void PIController_Init(PIController *pi)
{

	pi->integrator = 0.0;
	pi->prevError  = 0.0;

	pi->out = 0.0;

}

float PIController_Update(PIController *pi, float consigne, float measurement)
{

    float error = consigne - measurement;

    float proportional = pi->Kp * error;	//Proportional

    pi->integrator = pi->integrator + 0.5 * pi->Ki * pi->T * (error + pi->prevError);	//Integrator

    if (pi->integrator > pi->limMax_integrator) pi->integrator = pi->limMax_integrator; //anti-wind-up par ecretage
    else if (pi->integrator < pi->limMin_integrator) pi->integrator = pi->limMin_integrator;


    pi->out = proportional + pi->integrator;	//Output

    if (pi->out > pi->limMax_output) pi->out = pi->limMax_output;
    else if (pi->out < pi->limMin_output) pi->out = pi->limMin_output;

    pi->prevError       = error;

    return pi->out;

}

float PIPrint_Out(PIController *pi)
{
	return pi->out;
}
