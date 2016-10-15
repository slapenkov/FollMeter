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
#define DAC_POINTS 10
#define N_SAMPLES 100

/* Variables section */
static float freq_set[] = { 0.00, 0.12, 0.25, 0.50, 1.00, 2.00, 4.00, 8.00,
		16.00, 32.00, 64.00, 128.00, 256.00, 512.00, 00.00 }; //scan frequency set
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

/* Prototypes section */
void ProcessSensors(void);
void FreqScreenUpdate(int idx);

#endif /* INCLUDE_MAIN_H_ */
