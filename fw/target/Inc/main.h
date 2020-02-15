/* USER CODE BEGIN Header */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN_MODE_Pin GPIO_PIN_13
#define BTN_MODE_GPIO_Port GPIOC
#define BTN_MODE_EXTI_IRQn EXTI15_10_IRQn
#define BTN_SEL_Pin GPIO_PIN_14
#define BTN_SEL_GPIO_Port GPIOC
#define BTN_DOWN_Pin GPIO_PIN_15
#define BTN_DOWN_GPIO_Port GPIOC
#define BTN_UP_Pin GPIO_PIN_0
#define BTN_UP_GPIO_Port GPIOH
#define BTN_UP_EXTI_IRQn EXTI0_IRQn
#define OLED_CD_Pin GPIO_PIN_0
#define OLED_CD_GPIO_Port GPIOA
#define OLED_RESET_Pin GPIO_PIN_1
#define OLED_RESET_GPIO_Port GPIOA
#define LEN_VBAT_MEAS_Pin GPIO_PIN_2
#define LEN_VBAT_MEAS_GPIO_Port GPIOA
#define SFP_TX_FAULT_Pin GPIO_PIN_6
#define SFP_TX_FAULT_GPIO_Port GPIOA
#define SFP_TX_DIS_Pin GPIO_PIN_0
#define SFP_TX_DIS_GPIO_Port GPIOB
#define SFP_RS0_Pin GPIO_PIN_1
#define SFP_RS0_GPIO_Port GPIOB
#define SFP_PRES_Pin GPIO_PIN_2
#define SFP_PRES_GPIO_Port GPIOB
#define SFP_PRES_EXTI_IRQn EXTI2_IRQn
#define RET_INT_Pin GPIO_PIN_12
#define RET_INT_GPIO_Port GPIOB
#define RET_PWR_HEN_Pin GPIO_PIN_13
#define RET_PWR_HEN_GPIO_Port GPIOB
#define SFP_PWR_HEN_Pin GPIO_PIN_15
#define SFP_PWR_HEN_GPIO_Port GPIOB
#define P12V_OLED_HEN_Pin GPIO_PIN_8
#define P12V_OLED_HEN_GPIO_Port GPIOA
#define CHG_CHG_Pin GPIO_PIN_15
#define CHG_CHG_GPIO_Port GPIOA
#define CHG_ISET_Pin GPIO_PIN_5
#define CHG_ISET_GPIO_Port GPIOB
#define CHG_PG_Pin GPIO_PIN_6
#define CHG_PG_GPIO_Port GPIOB
#define CHG_PG_EXTI_IRQn EXTI9_5_IRQn
#define SFP_LOS_Pin GPIO_PIN_7
#define SFP_LOS_GPIO_Port GPIOB
#define SFP_RS1_Pin GPIO_PIN_9
#define SFP_RS1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
