/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stm32mp157dxx_cm4.h"

#define STM_OSPEED 0x2 // ~85Mhz at 50pF

GPIO_TypeDef * const digital_regs[] = {
    ['A' - 'A'] = GPIOA, GPIOB, GPIOC,
#ifdef GPIOD
    ['D' - 'A'] = GPIOD,
#endif
#ifdef GPIOE
    ['E' - 'A'] = GPIOE,
#endif
#ifdef GPIOF
    ['F' - 'A'] = GPIOF,
#endif
#ifdef GPIOG
    ['G' - 'A'] = GPIOG,
#endif
#ifdef GPIOH
    ['H' - 'A'] = GPIOH,
#endif
#ifdef GPIOI
    ['I' - 'A'] = GPIOI,
#endif
#ifdef GPIOZ
    ['Z' - 'A'] = GPIOZ,
#endif
};

// Enable a peripheral clock
void enable_pclock(uint32_t periph_base)
{
    // periph_base determines in which bitfield at wich position to set a bit
    // E.g. D2_AHB1PERIPH_BASE is the adress offset of the given bitfield
    if (periph_base < MCU_APB2_PERIPH_BASE) {
        if(periph_base <= LPTIM1_BASE) {
            uint32_t pos = (periph_base - TIM2_BASE) / 0x1000;
            RCC->MC_APB1ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base == WWDG1_BASE) {
            RCC->MC_APB1ENSETR |= (1<<0x1C); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base <= SPI3_BASE) {
            uint32_t pos = (periph_base - TIM2_BASE) / 0x1000;
            RCC->MC_APB1ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base == SPDIFRX_BASE) {
            RCC->MC_APB1ENSETR |= (1<<0x1A); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base <= UART5_BASE) {
            uint32_t pos = (periph_base - TIM2_BASE) / 0x1000;
            RCC->MC_APB1ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base <= I2C5_BASE) {
            uint32_t pos = ((periph_base + 0x3000) - TIM2_BASE) / 0x1000;
            RCC->MC_APB1ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base == CEC_BASE) {
            RCC->MC_APB1ENSETR |= (1<<0x1B); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base == DAC1_BASE) {
            RCC->MC_APB1ENSETR |= (1<<0x1D); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        } else if (periph_base <= UART8_BASE) {
            uint32_t pos = ((periph_base) - TIM2_BASE) / 0x1000;
            RCC->MC_APB1ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB1ENSETR;
        }
    } else if (periph_base < MCU_AHB2_PERIPH_BASE) {
        if(periph_base <= TIM8_BASE) {
            uint32_t pos = (periph_base - TIM1_BASE) / 0x1000;
            RCC->MC_APB2ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB2ENSETR;
        } if(periph_base == USART6_BASE) {
            RCC->MC_APB2ENSETR |= (1<<0x0D); // we assume it is not in APB1HENR
            RCC->MC_APB2ENSETR;
        } if(periph_base <= SPI4_BASE) {
            uint32_t pos = ((periph_base + 0x4000) - TIM1_BASE) / 0x1000;
            RCC->MC_APB2ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB2ENSETR;
        } if(periph_base <= TIM17_BASE) {
            uint32_t pos = ((periph_base - 0x4000) - TIM1_BASE) / 0x1000;
            RCC->MC_APB2ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_APB2ENSETR;
        } if(periph_base == SPI5_BASE) {
            RCC->MC_APB2ENSETR |= (1<<0x0A); // we assume it is not in APB1HENR
            RCC->MC_APB2ENSETR;
        }
    } else if (periph_base < MCU_AHB3_PERIPH_BASE) {
        if(periph_base <= DMAMUX1_BASE) {
            uint32_t pos = (periph_base - DMA1_BASE) / 0x1000;
            RCC->MC_AHB2ENSETR |= (1<<pos); // we assume it is not in APB1HENR
            RCC->MC_AHB2ENSETR;
        } if(periph_base <= ADC12_COMMON_BASE) {
            RCC->MC_AHB2ENSETR |= RCC_MC_AHB2ENSETR_ADC12EN;
            RCC->MC_AHB2ENSETR;
        }
    } else if (periph_base < MCU_AHB4_PERIPH_BASE) {
        uint32_t pos = (periph_base - MCU_AHB3_PERIPH_BASE) / 0x1000;
        RCC->MC_AHB3ENSETR |= (1<<pos);
        RCC->MC_AHB3ENSETR;
    } else if (periph_base < MCU_APB3_PERIPH_BASE) {
        uint32_t pos = ((periph_base - 0x2000) - MCU_AHB4_PERIPH_BASE) / 0x1000;
        RCC->MC_AHB4ENSETR |= (1<<pos);
        RCC->MC_AHB4ENSETR;
    } else if (periph_base < APB_DEBUG_PERIPH_BASE) {
        uint32_t pos = (periph_base - MCU_APB3_PERIPH_BASE) / 0x1000;
        RCC->MC_APB3ENSETR |= (1<<pos);
        RCC->MC_APB3ENSETR;
    } else if (periph_base < GPV_PERIPH_BASE) {
        if(periph_base == GPIOZ_BASE) {
            RCC->MC_AHB5ENSETR |= (1<<0x00); // we assume it is not in APB1HENR
            RCC->MC_AHB5ENSETR;
        }
    } else {
      //  uint32_t pos = (periph_base - MCU_APB5_PERIPH_BASE) / 0x1000;
       // RCC->MC_APB5ENSETR |= (1<<pos);
      //  RCC->MC_APB5ENSETR;
    }
}

// Enable a GPIO peripheral clock
void
gpio_clock_enable(GPIO_TypeDef *regs)
{
    enable_pclock((uint32_t)regs);
}

// Set the mode and extended function of a pin
void gpio_peripheral(uint32_t gpio, uint32_t mode, int pullup)
{
    GPIO_TypeDef *regs = digital_regs[GPIO2PORT(gpio)];

    // Enable GPIO clock
    gpio_clock_enable(regs);

    // Configure GPIO
    uint32_t mode_bits = mode & 0xf, func = (mode >> 4) & 0xf, od = mode >> 8;
    uint32_t pup = pullup ? (pullup > 0 ? 1 : 2) : 0;
    uint32_t pos = gpio % 16, af_reg = pos / 8;
    uint32_t af_shift = (pos % 8) * 4, af_msk = 0x0f << af_shift;
    uint32_t m_shift = pos * 2, m_msk = 0x03 << m_shift;

    regs->AFR[af_reg] = (regs->AFR[af_reg] & ~af_msk) | (func << af_shift);
    regs->MODER = (regs->MODER & ~m_msk) | (mode_bits << m_shift);
    regs->PUPDR = (regs->PUPDR & ~m_msk) | (pup << m_shift);
    regs->OTYPER = (regs->OTYPER & ~(1 << pos)) | (od << pos);
    regs->OSPEEDR = (regs->OSPEEDR & ~m_msk) | (STM_OSPEED << m_shift);
}

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


void clock_setup(void)
{
	NVIC_SetPriorityGrouping(0x03);

	  uint32_t mcudiv = 0;

	  /* Compute MCU frequency ---------------------------*/
	  mcudiv = (uint32_t)(RCC->MCUDIVR & RCC_MCUDIVR_MCUDIV );
	  if (mcudiv > RCC_MCUDIVR_MCUDIV_9)
	  {
	    mcudiv = RCC_MCUDIVR_MCUDIV_9;
	  }

/* Update the SystemCoreClock global variable */
SystemCoreClock = (HSI_VALUE >> ((uint32_t)(RCC->HSICFGR & RCC_HSICFGR_HSIDIV )))>> mcudiv;

/* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */

/* Configure the SysTick to have interrupt in 1ms time basis*/
SysTick_Config(SystemCoreClock /(1000U ));

/* Configure the SysTick IRQ priority */

	//  prioritygroup = NVIC_GetPriorityGrouping();

NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), TICK_INT_PRIORITY, 0U));
//   HAL_NVIC_SetPriority(SysTick_IRQn, TICK_INT_PRIORITY, 0U);
  uwTickPrio = TICK_INT_PRIORITY;


    /* Enable access to RTC and backup registers */
    SET_BIT(PWR->CR1, PWR_CR1_DBP);
    MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV, (uint32_t)(RCC_BDCR_LSEDRV_1));      // 允许修改LSE;

    /* Disable HSEON before configuring the HSE --------------*/
    WRITE_REG(RCC->OCENCLRR, RCC_OCENCLRR_HSEON);

    /* Clear remaining bits */
    WRITE_REG(RCC->OCENCLRR, (RCC_OCENCLRR_DIGBYP | RCC_OCENSETR_HSEBYP));

    /* Enable oscillator */
    SET_BIT(RCC->OCENSETR, RCC_OCENSETR_HSEON);

    /* Enable the Internal Low Speed oscillator (LSI). */
    SET_BIT(RCC->RDLSICR, RCC_RDLSICR_LSION);

    WRITE_REG(RCC->PLL1CR, (0x0));
    WRITE_REG(RCC->PLL1CFGR1, (0x10031));
    WRITE_REG(RCC->PLL1CFGR2, (0x10100));
    WRITE_REG(RCC->PLL1FRACR, (0x0));

    WRITE_REG(RCC->PLL2CR, (0x0));
    WRITE_REG(RCC->PLL2CFGR1, (0x10063));
    WRITE_REG(RCC->PLL2CFGR2, (0x10101));
    WRITE_REG(RCC->PLL2FRACR, (0x0));

    WRITE_REG(RCC->PLL3CR, (0x73));
    WRITE_REG(RCC->PLL3CFGR1, (0x1010033));
    WRITE_REG(RCC->PLL3CFGR2, (0x10102));
    WRITE_REG(RCC->PLL3FRACR, (0x14000));

    WRITE_REG(RCC->PLL4CR, (0x0));
    WRITE_REG(RCC->PLL4CFGR1, (0x10031));
    WRITE_REG(RCC->PLL4CFGR2, (0x0));
    WRITE_REG(RCC->PLL4FRACR, (0x0));
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

#if 1
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	NVIC_SetPriorityGrouping(0x03);

  HAL_Init();

    SystemClock_Config();

#else

  clock_setup();

//  SystemClock_Config();
#endif

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  /* printf("请输入字符，并按下回车键结束\r\n"); */
  HAL_UART_Receive_IT(&huart4,&RxBuffer,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t que = HAL_RCC_GetSystemCoreClockFreq();
  printf("\r\n SystemCoreClockFreq:0x%d\r\n",que);
//   wwdg_init(0x7F, 0x5F, WWDG_PRESCALER_16); /* 计数器值为7f,窗口寄存器为5f,分频数为16 */

  printf("\r\n UART8->BRR:0x%X\r\n",UART8->BRR);
  printf("\r\n RCC->UART78CKSELR: 0x%X\r\n",RCC->UART78CKSELR);

  printf("\r\n RCC->PLL1CR: 0x%X\r\n",RCC->PLL1CR);
  printf("\r\n RCC->PLL1CFGR1: 0x%X\r\n",RCC->PLL1CFGR1);
  printf("\r\n RCC->PLL1CFGR2: 0x%X\r\n",RCC->PLL1CFGR2);
  printf("\r\n RCC->PLL1FRACR: 0x%X\r\n",RCC->PLL1FRACR);

  printf("\r\n RCC->PLL2CR: 0x%X\r\n",RCC->PLL2CR);
  printf("\r\n RCC->PLL2CFGR1: 0x%X\r\n",RCC->PLL2CFGR1);
  printf("\r\n RCC->PLL2CFGR2: 0x%X\r\n",RCC->PLL2CFGR2);
  printf("\r\n RCC->PLL2FRACR: 0x%X\r\n",RCC->PLL2FRACR);

  printf("\r\n RCC->PLL3CR: 0x%X\r\n",RCC->PLL3CR);
  printf("\r\n RCC->PLL3CFGR1: 0x%X\r\n",RCC->PLL3CFGR1);
  printf("\r\n RCC->PLL3CFGR2: 0x%X\r\n",RCC->PLL3CFGR2);
  printf("\r\n RCC->PLL3FRACR: 0x%X\r\n",RCC->PLL3FRACR);

  printf("\r\n RCC->PLL4CR: 0x%X\r\n",RCC->PLL4CR);
  printf("\r\n RCC->PLL4CFGR1: 0x%X\r\n",RCC->PLL4CFGR1);
  printf("\r\n RCC->PLL4CFGR2: 0x%X\r\n",RCC->PLL4CFGR2);
  printf("\r\n RCC->PLL4FRACR: 0x%X\r\n",RCC->PLL4FRACR);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	 //HAL_UART_Receive_IT(&huart4,&RxBuffer,1);
	 printf("\nPlease enter characters and press enter to end\r\n");
	 HAL_Delay(6000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSIDivValue = RCC_HSI_DIV1;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLL3SOURCE_HSE;
  RCC_OscInitStruct.PLL3.PLLM = 2;
  RCC_OscInitStruct.PLL3.PLLN = 52;
  RCC_OscInitStruct.PLL3.PLLP = 3;
  RCC_OscInitStruct.PLL3.PLLQ = 2;
  RCC_OscInitStruct.PLL3.PLLR = 2;
  RCC_OscInitStruct.PLL3.PLLRGE = RCC_PLL3IFRANGE_1;
  RCC_OscInitStruct.PLL3.PLLFRACV = 2048;
  RCC_OscInitStruct.PLL3.PLLMODE = RCC_PLL_FRACTIONAL;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** RCC Clock Config
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_ACLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3|RCC_CLOCKTYPE_PCLK4
                              |RCC_CLOCKTYPE_PCLK5;
  RCC_ClkInitStruct.AXISSInit.AXI_Clock = RCC_AXISSOURCE_HSI;
  RCC_ClkInitStruct.AXISSInit.AXI_Div = RCC_AXI_DIV1;
  RCC_ClkInitStruct.MCUInit.MCU_Clock = RCC_MCUSSOURCE_PLL3;
  RCC_ClkInitStruct.MCUInit.MCU_Div = RCC_MCU_DIV1;
  RCC_ClkInitStruct.APB4_Div = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5_Div = RCC_APB5_DIV1;
  RCC_ClkInitStruct.APB1_Div = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2_Div = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB3_Div = RCC_APB3_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Set the HSE division factor for RTC clock
  */
  MODIFY_REG( RCC->RTCDIVR, RCC_RTCDIVR_RTCDIV , 0);
//  __HAL_RCC_RTC_HSEDIV(1);
}

/* USER CODE BEGIN 4 */

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
// 本实验使用的是UART4，如果使用的是其它串口，则将UART4改为对应的串口即可
    while ((UART4->ISR & 0X40) == 0);
    UART4->TDR = (uint8_t) ch;
    return ch;
}

/*
int __io_putchar(int ch)
{
    //具体哪个串口可以更改USART4为其它串口
    while ((UART4->ISR & 0X40) == 0); //循环发送,直到发送完毕
    UART4->TDR = (uint8_t) ch;
    return ch;
}
*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
