#include "main.h"

/* Consts */
const uint16_t Sine12bit[DAC_POINTS] = { 2048, 3251, 3995, 3996, 3253, 2051,
		847, 101, 98, 839 };

const float dcos[DAC_POINTS] = { 1, 1, 1, -1, -1, -1, -1, -1, 1, 1 };
const float dsin[DAC_POINTS] = { 1, 1, 1, 1, 1, -1, -1, -1, -1, -1 };

int main(void) {

	// Set up 40 MHz Core Clock using HSI (8Mhz) with PLL x 5
	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_5);
	RCC_PLLCmd(ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // Select the PLL as clock source.
	SystemCoreClockUpdate();

	//LCD init
	LCD_init();

	//preload section
	LCD_string("Impedance scanner", 15, 56, FONT_TYPE_5x8,
			INVERSE_TYPE_NOINVERSE);
	LCD_string("FIS-01", 45, 40, FONT_TYPE_5x15, INVERSE_TYPE_NOINVERSE);
	LCD_string("Firmware v0.0.1", 15, 32, FONT_TYPE_5x8,
			INVERSE_TYPE_NOINVERSE);
	LCD_string("Dnipro 2016", 30, 24, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("Loading,please wait...", 0, 0, FONT_TYPE_5x8,
			INVERSE_TYPE_NOINVERSE);

	//wait
	tool_delay_ms(3000);

	//main interface section
	sprintf(strFreqPrev, "%06.2f", freq_set[freq_idx - 1]);
	sprintf(strFreqSel, "%06.2f", freq_set[freq_idx]);
	sprintf(strFreqNext, "%06.2f", freq_set[freq_idx + 1]);

	LCD_clear(0);
	LCD_string("<", 0, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string(strFreqPrev, 5, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string(strFreqSel, 48, 56, FONT_TYPE_5x8, INVERSE_TYPE_INVERSE);
	LCD_string(strFreqNext, 92, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string(">", 122, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);

	LCD_line(LINE_TYPE_DOT, 0, 52, 127, 52);

	LCD_string("  g,uSm:", 0, 40, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("***.***", 50, 40, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("  b,uSm:", 0, 32, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("***.***", 50, 32, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("|Y|,uSm:", 0, 24, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("***.***", 50, 24, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string("Phi,rad:", 0, 16, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE); //phase
	LCD_string("***.***", 50, 16, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);

	LCD_line(LINE_TYPE_DOT, 0, 12, 127, 12);

	LCD_string("...processing...", 15, 0, FONT_TYPE_5x8,
			INVERSE_TYPE_NOINVERSE); //processing message and scanner trend area

	/* Init STMTouch driver */
	//TSL_user_Init();
	//Peripheral configure
	adcRdy = 0;
	InitMeasurements();

	while (1) {
		//touch keys processing
		/*if (TSL_user_Action() == TSL_STATUS_OK) {
		 ProcessSensors(); // Execute sensors related tasks
		 //process key code action
		 if ((prev_key == 0) & (key != 0)) {
		 //key pressed detected
		 switch (key) {
		 case FIRST_KEY:
		 freq_idx--;
		 if (freq_idx == 0)
		 freq_idx++;
		 break;
		 case SECOND_KEY:
		 break;
		 case THIRD_KEY:
		 break;
		 case FOURTH_KEY:
		 freq_idx++;
		 if (freq_idx + 1 == sizeof(freq_set) / sizeof(float))
		 freq_idx--;
		 break;
		 }
		 //update screen
		 FreqScreenUpdate(freq_idx);
		 //reset measurement
		 InitMeasurements();
		 }
		 prev_key = key; //store previous state
		 }*/
	} // Infinity loop
}

/*
 * ADC+DMA Config
 * */
void ADC_Config(void) {
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	/* ADC1 DeInit */
	ADC_DeInit(ADC1);

	/* Enable ADC and GPIO clocks ****************************************/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Configure ADC1 Channel13 pin as analog input ******************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continous mode with a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge =
	ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO; //trigger from tim2
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* Convert the ADC1 Channel 13 with 1.5 Cycles as sampling time */
	ADC_ChannelConfig(ADC1, ADC_Channel_13, ADC_SampleTime_1_5Cycles); //@todo

	/* Enable DMA request after last transfer (OneShot-ADC mode) */
	//ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
	/* Enable ADCperipheral[PerIdx] */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Wait the ADCEN falg */
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
		;
	/* ADC1 regular Software Start Conv */
	ADC_StartOfConversion(ADC1);

	/* DMA1 Stream1 channel1 configuration **************************************/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &adc_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = sizeof(adc_buf) / sizeof(uint16_t);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	// Enable DMA1 Channel Transfer Complete interrupt
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	//Enable DMA1 channel IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* ADC1 regular Software Start Conv */
	ADC_StartOfConversion(ADC1);
}

/*
 * DAC+DMA Configure
 * */
void DAC_Config(void) {
	GPIO_InitTypeDef gpio;
	DMA_InitTypeDef dma;

	/* DMA1 clock enable (to be used with DAC) */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* GPIOA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure PA.04 (DAC_OUT1) as analog */
	gpio.GPIO_Pin = GPIO_Pin_4;
	gpio.GPIO_Mode = GPIO_Mode_AN;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio);

	/* DAC channel1 Configuration */
	DAC_DeInit();
	dac.DAC_Trigger = DAC_Trigger_T2_TRGO; //trigger by tim2
	dac.DAC_WaveGeneration = DAC_WaveGeneration_None;
	dac.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	/* DAC Channel1 Init */
	DAC_Init(DAC_Channel_1, &dac);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	/* DMA1 channel3 configuration */
	DMA_DeInit(DMA1_Channel3);
	dma.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
	dma.DMA_MemoryBaseAddr = (uint32_t) &Sine12bit;
	dma.DMA_DIR = DMA_DIR_PeripheralDST;
	dma.DMA_BufferSize = DAC_POINTS;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &dma);

	/* Enable DMA1 Channel3 */
	DMA_Cmd(DMA1_Channel3, ENABLE);

	/* Enable DMA for DAC Channel2 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/*
 * TIM2 Configure
 * */
void TIM2_Config(void) {
	TIM_TimeBaseInitTypeDef tim;

	/* TIM2 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructInit(&tim);
	tim.TIM_Period = tmr_set[freq_idx];
	tim.TIM_Prescaler = 0x0;
	tim.TIM_ClockDivision = 0x0;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &tim);

	/* TIM2 TRGO selection */
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

/*
 * Init measurements - start timer and init ADC DMA interrupt
 * */
void InitMeasurements(void) {
	TIM2_Config();
	DAC_Config();
	ADC_Config();
}

/*
 * Process measurements results
 * @todo */
void ProcessMeasurements(void) {
	//clear sum buffer
	for (int i = 0; i < DAC_POINTS; i++) {
		sum_buf[i] = 0;
	}
	//scan adc buffer and add corresponds to sum buffer
	for (int i = 0; i < (DAC_POINTS * N_SAMPLES); i++) {
		sum_buf[i % DAC_POINTS] += (1.0 * ((float) (adc_buf[i] - 128)) / 256);
	}
	//averaging
	for (int i = 0; i < DAC_POINTS; i++) {
		sum_buf[i] = -sum_buf[i] / (N_SAMPLES * FB_RESISTOR);
	}
	//combining with test samples for d1 and d2
	d1 = 0;
	d2 = 0;

	for (int i = 0; i < DAC_POINTS; i++) {
		d1 += dcos[i] * sum_buf[i];
		d2 += dsin[i] * sum_buf[i];
	}

	d1 /= DAC_POINTS;
	d2 /= DAC_POINTS;

	//amplitude and phase
	amplitude = sqrt(d1 * d1 + d2 * d2);
	phase = -atan2(d2, d1);
}

/*
 * Update results screen
 * */
void UpdateResultsScreen(void) {
	char temp[] = "";
	//real part
	sprintf(temp, "%09.3f", d1);
	LCD_string(temp, 74, 40, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	//imagine part
	sprintf(temp, "%09.3f", d2);
	LCD_string(temp, 74, 32, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	//ampl
	sprintf(temp, "%09.3f", amplitude);
	LCD_string(temp, 74, 24, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	//phase
	sprintf(temp, "%09.3f", phase);
	LCD_string(temp, 74, 16, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
}

/**
 * @brief  Manage the activity on sensors when touched/released (example)
 * @param  None
 * @retval None
 */
void ProcessSensors(void) {
	key = 0;
	if ((MyLinRots[0].p_Data->StateId == TSL_STATEID_DETECT)
			|| (MyLinRots[0].p_Data->StateId == TSL_STATEID_DEB_RELEASE_DETECT)) {

		if (MyLinRots[0].p_Data->Position > 0) {
			key = FIRST_KEY;
		}

		if (MyLinRots[0].p_Data->Position >= 48) {
			key = SECOND_KEY;
		}

		if (MyLinRots[0].p_Data->Position >= 80) {
			key = THIRD_KEY;
		}

		if (MyLinRots[0].p_Data->Position >= 112) {
			key = FOURTH_KEY;
		}
	}
}

/*
 * Frequency screen update by index
 * */
void FreqScreenUpdate(int idx) {
	char temp[] = { 0x20 };

	//prepare strings
	sprintf(strFreqPrev, "%06.2f", freq_set[freq_idx - 1]);
	sprintf(strFreqSel, "%06.2f", freq_set[freq_idx]);
	sprintf(strFreqNext, "%06.2f", freq_set[freq_idx + 1]);

	//redraw screen
	if ((idx - 1) > 0) {
		temp[0] = 0x3c;
	}
	LCD_string(temp, 0, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string(strFreqPrev, 5, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	LCD_string(strFreqSel, 48, 56, FONT_TYPE_5x8, INVERSE_TYPE_INVERSE);
	LCD_string(strFreqNext, 92, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
	temp[0] = 0x20;
	if ((idx + 2) < sizeof(freq_set) / sizeof(float)) {
		temp[0] = 0x3e;
	}
	LCD_string(temp, 122, 56, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);

}

/**
 * @brief  Executed when a sensor is in Off state
 * @param  None
 * @retval None
 */
void MyLinRots_OffStateProcess(void) {
	/* Add here your own processing when a sensor is in Off state */
}

/**
 * @brief  Executed at each timer interruption (option must be enabled)
 * @param  None
 * @retval None
 */
void TSL_CallBack_TimerTick(void) {
}

/**
 * @brief  Executed when a sensor is in Error state
 * @param  None
 * @retval None
 */
void MyLinRots_ErrorStateProcess(void) {
	/* Add here your own processing when a sensor is in Error state */
	TSL_linrot_SetStateOff();
	while (1) {
	}
}

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in 10 ms.
 * @retval None
 */
void Delay(__IO uint32_t nTime) {
	TimingDelay = nTime;

	while (TimingDelay != 0)
		;
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void TimingDelay_Decrement(void) {
	if (TimingDelay != 0x00) {
		TimingDelay--;
	}
}

/*
 * DMA ISR process
 * */
void DMA_ISR(void) {
	if (DMA_GetITStatus(DMA1_IT_TC1)) {
		DMA_Cmd(DMA1_Channel1, DISABLE);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		char temp[] = "";
		/*sprintf(temp, "Remains %i     ", DMA_GetCurrDataCounter(DMA1_Channel1));
		 LCD_string(temp, 0, 0, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE); //processing message and scanner trend area*/
		DMA_SetCurrDataCounter(DMA1_Channel1, DAC_POINTS * N_SAMPLES);
		DMA_SetCurrDataCounter(DMA1_Channel3, DAC_POINTS);
		ProcessMeasurements();
		//LCD_clear(0);
		//real part
		sprintf(temp, "%07.3f", d1);
		LCD_string(temp, 49, 40, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		//imagine part
		sprintf(temp, "%07.3f", d2);
		LCD_string(temp, 49, 32, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		//ampl
		sprintf(temp, "%07.3f", amplitude);
		LCD_string(temp, 49, 24, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		//phase
		sprintf(temp, "%07.3f", phase);
		LCD_string(temp, 49, 16, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		//prepare draw area
		LCD_string("      ", 94, 40, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		LCD_string("      ", 94, 32, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		LCD_string("      ", 94, 24, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		LCD_string("      ", 94, 16, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE);
		//signal draw
		uint8_t x = 94, y, xp, yp;
		xp = x;
		yp = (uint8_t) (64 * sum_buf[0] / 10) + 32;
		for (uint8_t i = 1; i < 10; i++) {
			x += 3;
			y = (uint8_t) (64 * sum_buf[i] / 10) + 32;
			y = (y > 48) ? 48 : y;
			y = (y < 15) ? 15 : y;
			LCD_line(LINE_TYPE_BLACK, xp, yp, x, y);
			xp = x;
			yp = y;
		}

		DMA_Cmd(DMA1_Channel1, ENABLE);
		DMA_Cmd(DMA1_Channel3, ENABLE);
		/* Clear DMA TC flag */
		DMA_ClearITPendingBit(DMA1_IT_GL1);
		/* ADC1 regular Software Start Conv */
		ADC_StartOfConversion(ADC1);
	}
}
