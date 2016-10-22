/**
 ******************************************************************************
 * @file    Projects\Demonstration\stm32f0xx_it.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    17-January-2014
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
//#include "usb_dcd_int.h"
//#include "usbd_hid_core.h"

/** @addtogroup STM32F072B_DISCOVERY
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t ButtonPressed = 0x00;
extern __IO uint32_t Gv_EOA;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
	//TimingDelay_Decrement();
	TSL_tim_ProcessIT();
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

void USB_IRQHandler(void) {
	/*USB_Istr();*/
}

/**
 * @brief  This function handles Touch Sensing Controller Handler.
 * @param  None
 * @retval None
 */
void TSC_IRQHandler(void) {
#if TSLPRM_TSC_IODEF > 0 // Default = Input Floating
	/* Set IO default in Output PP Low to discharge all capacitors */
	TSC->CR &= (uint32_t)(~(1 << 4));
#endif
	TSC->ICR |= 0x03; /* Clear EOAF and MCEF flags */
	Gv_EOA = 1; /* To inform the main loop routine of the End Of Acquisition */
}
/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Channel1_IRQHandler(void) {
	NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	if (DMA_GetITStatus(DMA1_IT_TC1)) {
		StopMeasurements();
		char temp[] = "";
		sprintf(temp, "Remains %i     ", DMA_GetCurrDataCounter(DMA1_Channel1));
		LCD_string(temp, 0, 0, FONT_TYPE_5x8, INVERSE_TYPE_NOINVERSE); //processing message and scanner trend area
		ProcessMeasurements();
		UpdateResultsScreen();
		/* Clear DMA TC flag */
		DMA_ClearITPendingBit(DMA1_IT_GL1);
		InitMeasurements(); //start again
	}
}

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
