/**
 ******************************************************************************
 * @file    rtc_hal.c
 * @author  Satish Nair, Brett Walach
 * @version V1.0.0
 * @date    12-Sept-2014
 * @brief
 ******************************************************************************
  Copyright (c) 2013-14 Spark Labs, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rtc_hal.h"
#include "stm32f10x_rtc.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

void HAL_RTC_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RTC_IRQ_PRIORITY;			//OLD: 0x01
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;								//OLD: 0x01
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RTCALARM_IRQ_PRIORITY;		//OLD: 0x01
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;								//OLD: 0x02
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Check if the StandBy flag is set */
    if(PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
    {
        /* System resumed from STANDBY mode */

        /* Clear StandBy flag */
        PWR_ClearFlag(PWR_FLAG_SB);

        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();

        /* No need to configure the RTC as the RTC configuration(clock source, enable,
	       prescaler,...) is kept after wake-up from STANDBY */
    }
    else
    {
        /* StandBy flag is not set */

        /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);

        /* Wait till LSE is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {
            //Do nothing
        }

        /* Select LSE as RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);

        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Set RTC prescaler: set RTC period to 1sec */
        RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }

    /* Enable the RTC Second and RTC Alarm interrupt */
    RTC_ITConfig(RTC_IT_SEC | RTC_IT_ALR, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

time_t HAL_RTC_Get_UnixTime(void)
{
  return (time_t)RTC_GetCounter();
}

void HAL_RTC_Set_UnixTime(time_t value)
{
  RTC_WaitForLastTask();
  RTC_SetCounter((uint32_t)value);
  RTC_WaitForLastTask();
}

void HAL_RTC_Set_UnixAlarm(time_t value)
{
  /* Set the RTC Alarm */
  RTC_SetAlarm(RTC_GetCounter() + (uint32_t)value);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
