/*
 * main.h
 *
 *  Created on: 13 окт. 2016 г.
 *      Author: gray
 */

#ifndef INCLUDE_MAIN_H_
#define INCLUDE_MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include "stm32f0xx.h"
#include "uc1601s.h"
#include "tools.h"
#include "tsl_types.h"
#include "tsl_user.h"

/* Defines section */
#define DAC_DHR12R1_ADDRESS      0x40007408
#define DAC_DHR8R2_ADDRESS       0x4000741C
#define ADC1_DR_ADDRESS     ((uint32_t)0x40012440)

#define DAC_POINTS 10
#define N_SAMPLES 100

/* Variables section */
static float freq_set[] = { 0.00, 0.12, 0.25, 0.50, 1.00, 2.00, 4.00, 8.00,
		16.00, 32.00, 64.00, 128.00, 256.00, 512.00, 00.00 }; //scan frequency set
//static uint32_t tmr_set[] = {0, }
static int freq_idx = 4; //default frequency index

static char strFreqPrev[6];
static char strFreqSel[6];
static char strFreqNext[6];

static int key = 0;
static int prev_key = 0;

enum {
	FIRST_KEY = 1, SECOND_KEY, THIRD_KEY, FOURTH_KEY
};

static uint16_t dac_buf[DAC_POINTS];

static uint16_t adc_buf[DAC_POINTS * N_SAMPLES];

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

//uint8_t i = 0;

DAC_InitTypeDef dac;

/* Prototypes section */
void ADC_Config(void);
void DAC_Config(void);
void TIM2_Config(void);
void ProcessSensors(void);
void FreqScreenUpdate(int idx);
void InitMeasurements(void);
void StopMeasurements(void);
void ProcessMeasurements(void);
void UpdateResultsScreen(void);

#endif /* INCLUDE_MAIN_H_ */
