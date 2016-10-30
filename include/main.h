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
#include "math.h"

/* Defines section */
#define DAC_DHR12R1_ADDRESS      0x40007408
#define DAC_DHR8R2_ADDRESS       0x4000741C
#define ADC1_DR_ADDRESS     ((uint32_t)0x40012440)

#define DAC_POINTS 10
#define N_SAMPLES 100

#define FB_RESISTOR 0.2 //in MOhms

/* Variables section */
static double freq_set[] = { 0.00, 0.12, 0.25, 0.50, 1.00, 2.00, 4.00, 8.00,
		16.00, 31.40, 62.70, 125.60, 251.10, 502.20, 00.00 }; //scan frequency set
static uint32_t tmr_set[] = { 0, 32150, 16070, 8035, 4017, 2008, 1002, 501, 250,
		127, 63, 31, 15, 1, 0 };
static int freq_idx = 5; //default frequency index

static char strFreqPrev[6];
static char strFreqSel[6];
static char strFreqNext[6];

static int key = 0;
static int prev_key = 0;

static __IO uint32_t adcRdy;

enum {
	FIRST_KEY = 1, SECOND_KEY, THIRD_KEY, FOURTH_KEY
};

//static uint16_t dac_buf[DAC_POINTS];

volatile uint16_t adc_buf[DAC_POINTS * N_SAMPLES];

volatile double sum_buf[DAC_POINTS];

static double d1 = 0, d2 = 0;

static double amplitude = 0, phase = 0;

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

DAC_InitTypeDef dac;

/* Prototypes section */
void ADC_Config(void);
void DAC_Config(void);
void TIM2_Config(void);
void ProcessSensors(void);
void FreqScreenUpdate(int idx);
void InitMeasurements(void);
void ProcessMeasurements(void);
void UpdateResultsScreen(void);
void DMA_ISR(void);

#endif /* INCLUDE_MAIN_H_ */
