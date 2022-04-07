/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mpu6050.h"
#include "retarget.h"
#include "test_freq.h"
#include "tb6612.h"
#include "pid.h"
#include "encoder.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
MPU6050_t mpu6050;
TB6612_Datatype tb6612;
PID_Datatype balance;
float target_degree = 0;
PID_Datatype speed;
float target_speed = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void set_tb6612();

void set_pid();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM4_Init();
    MX_USART3_UART_Init();
    MX_I2C1_Init();
    MX_TIM3_Init();
    /* USER CODE BEGIN 2 */
    while (MPU6050_Init(&hi2c1) == 1); // 初始化MPU6050
    RetargetInit(&huart3); // 初始化printf重载

    set_tb6612(); // 配置tb6612初始化结构体
    tb6612_init(&tb6612); // 初始化tb6612

    set_pid(); // 配置pid结构体

    encoder_init(); // 初始化编码器
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        test_freq_begin();

        static int u1 = 0;
        MPU6050_Read_All(&hi2c1, &mpu6050);
        u1 = (int) pid_get_u(&balance, target_degree, mpu6050.KalmanAngleX);

        static int u2 = 0;
        static float motorA, motorB, avg;
        encoder_get_speed(&motorA, &motorB);
        avg = (motorA + motorB) / 2;
        u2 = (int) pid_get_u(&speed, target_speed, avg);

        tb6612_set_value(&tb6612, MOTOR_A, u1 + u2);
        tb6612_set_value(&tb6612, MOTOR_B, u1 + u2);

        HAL_Delay(5);
        uint32_t freq = test_freq_end();

        printf("motorA:%f\tmotorB:%f\tfreq:%lu\n", motorA, motorB, freq);
//        printf("X:%.1f\tu1:%d\tfreq:%lu\n", mpu6050.KalmanAngleX, u1, freq);

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void set_tb6612() {
    tb6612.AIN1_port = AIN1_GPIO_Port;
    tb6612.AIN1_pin = AIN1_Pin;
    tb6612.AIN2_port = AIN2_GPIO_Port;
    tb6612.AIN2_pin = AIN2_Pin;
    tb6612.BIN1_port = BIN2_GPIO_Port;
    tb6612.BIN1_pin = BIN2_Pin;
    tb6612.BIN2_port = BIN1_GPIO_Port;
    tb6612.BIN2_pin = BIN1_Pin;

    tb6612.PWM_A = &htim1;
    tb6612.PWM_A_Channel = TIM_CHANNEL_1;
    tb6612.PWM_B = &htim1;
    tb6612.PWM_B_Channel = TIM_CHANNEL_4;
};

void set_pid() {
    balance.Kp = 360; //360  回复力
    balance.Ki = 0;
    balance.Kd = 1600; //1600  阻尼力
    balance.max_integral = 720;
    balance.min_integral = -720;
    balance.max_u = 7200;
    balance.min_u = -7200;
    balance.integral = 0;
    balance.last_error = 0;

    speed.Kp = -700; // -700
    speed.Ki = -0; //-3.5 速度的积分就是距离，故速度积分误差可以让小车回到原位
    speed.Kd = 0; //
    speed.max_integral = 720;
    speed.min_integral = -720;
    speed.max_u = 7200;
    speed.min_u = -7200;
    speed.integral = 0;
    speed.last_error = 0;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
