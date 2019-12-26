#include "common/sfp_hw.h"
#include "common/event.h"
#include "common/sfp.h"
#include "common/util.h"
#include "common/m88e111.h"
#include "m88e111_trg.h"
#include "sfp_trg.h"
#include "os.h"
#include <string.h>

static I2C_HandleTypeDef *hi2c = 0;
static uint8_t power_on = 0;

void sfp_trg_init(I2C_HandleTypeDef *h)
{
    hi2c = h;
    m88e111_trg_init(h);
}

void sfp_trg_set_power(uint8_t on)
{
    power_on = on;
    HAL_GPIO_WritePin(SFP_PWR_HEN_GPIO_Port, SFP_PWR_HEN_Pin, on);
}

uint8_t sfp_trg_is_present(void)
{
    return HAL_GPIO_ReadPin(SFP_PRES_GPIO_Port, SFP_PRES_Pin) == GPIO_PIN_RESET;
}

void sfp_trg_i2c_disable(void)
{
    __HAL_I2C_DISABLE(hi2c);
}

void sfp_trg_i2c_enable(void)
{
    __HAL_I2C_ENABLE(hi2c);
}

uint8_t sfp_hw_try_lock(void)
{
    return xSemaphoreTake(sfp_mutex, 10 / portTICK_PERIOD_MS);
}

void sfp_hw_unlock(void)
{
    xSemaphoreGive(sfp_mutex);
}

void sfp_hw_req(uint8_t op)
{
}

uint8_t sfp_hw_read(uint8_t i2c_addr, uint8_t mem_addr, uint8_t len, void *dest)
{
    HAL_StatusTypeDef r = HAL_I2C_Mem_Read(hi2c, i2c_addr, mem_addr, 1, dest, len, 1000);
    if (r == HAL_ERROR) {
        return SFP_READ_ERR;
    }
    else if (r == HAL_TIMEOUT) {
        return SFP_READ_TIMEOUT;
    }
    else if (r == HAL_BUSY) {
        return SFP_READ_ERR;
    }
    else {
        return SFP_READ_OK;
    }
}

static void swap2(void *p)
{
    uint8_t *p8 = p;
    uint8_t t = p8[0];
    p8[0] = p8[1];
    p8[1] = t;
}


static void swap4(void *p)
{
    uint8_t *p8 = p;
    uint8_t t = p8[0];
    p8[0] = p8[3];
    p8[3] = t;
    t = p8[1];
    p8[1] = p8[2];
    p8[2] = t;
}

static void fix_cal(sfp_alarm_cal_t *a)
{
    swap4(&a->rx_pwr4);
    swap4(&a->rx_pwr3);
    swap4(&a->rx_pwr2);
    swap4(&a->rx_pwr1);
    swap4(&a->rx_pwr0);
}


static uint8_t enter_sleep = 0;


static uint8_t auto_on = 1;

void sfp_hw_set_auto_on(uint8_t v)
{
    auto_on = v;
}

uint8_t sfp_hw_get_auto_on()
{
    return auto_on;
}

void sfp_trg_enter_sleep(void)
{
    if (sfp_state_get() == SFP_STATE_NOT_PRESENT) {
        if (auto_on)
            HAL_NVIC_EnableIRQ(EXTI2_IRQn);
        else
            HAL_NVIC_DisableIRQ(EXTI2_IRQn);
        return;
    }
    enter_sleep = 1;
    while (enter_sleep != 2) {
        vTaskDelay(10);
    }
}

void sfp_trg_exit_sleep(void)
{
    enter_sleep = 0;
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    xTaskNotifyGive(sfp_task);
}

static void sfp_trg_scan()
{
    vTaskDelay(125 / portTICK_PERIOD_MS);
    sfp_i2c_scan_reset();
    sfp_i2c_scan_t *res = sfp_i2c_scan_get();
    uint8_t res_cur = 0;
    for (uint8_t addr = 0; addr < 128; addr++) {
        if (((addr >> 3) & 0xf) == 0)
            continue;
        if (((addr >> 3) & 0xf) == 0xf)
            continue;
        uint8_t real_addr = addr << 1;
        if (HAL_I2C_IsDeviceReady(hi2c, real_addr, 2, 20) == HAL_OK) {
            res->addrs[res_cur] = addr;
            res_cur++;
        }

        if (res_cur == ARRAY_SIZE(res->addrs))
            break;
    }
}

uint16_t rev_88e111;

void sfp_task_code(void *param)
{
    while (1) {

        /*vTaskDelay(100/portTICK_PERIOD_MS);
        HAL_GPIO_WritePin(LEN_VBAT_MEAS_GPIO_Port, LEN_VBAT_MEAS_Pin, GPIO_PIN_RESET);
        vTaskDelay(1/portTICK_PERIOD_MS);
        HAL_ADC_Start(&hadc);
        HAL_ADC_PollForConversion(&hadc, 1000);
        uint16_t value = HAL_ADC_GetValue(&hadc);
        //xQueueSend(test_queue, &value, 0);

        HAL_GPIO_WritePin(LEN_VBAT_MEAS_GPIO_Port, LEN_VBAT_MEAS_Pin, GPIO_PIN_SET);*/

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (!sfp_trg_is_present() || enter_sleep != 0)
            continue;
        sfp_state_set(SFP_STATE_PRESENT);
        HAL_NVIC_DisableIRQ(EXTI2_IRQn);
        sfp_trg_i2c_disable();
        sfp_trg_set_power(1);
        sfp_hw_set_tx_dis(0);

        send_event(EVENT_SFP, EVENT_SFP_INSERTED);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        sfp_trg_i2c_enable();
        sfp_trg_scan();
        xSemaphoreTake(sfp_mutex, portMAX_DELAY);
        uint8_t r = sfp_hw_read(0xa0, 0, sizeof(sfp_sid_t), sfp_sid_get());
        if (r == SFP_READ_OK) {
            if (sfp_i2c_scan_has_addr8(0xa2)) {
                uint8_t r_alarm = sfp_hw_read(0xa2, 0, sizeof(sfp_alarm_cal_t), sfp_alarm_cal_get());
                if (r_alarm == SFP_READ_OK) {
                    fix_cal(sfp_alarm_cal_get());
                }
            }
            if (sfp_i2c_scan_has_addr8(SFP_I2C_ADDR_88E111)) {
                if (m88e111_check()) {
                    sfp_model_set(SFP_MODEL_88E111);
                }
            }
        }

        xSemaphoreGive(sfp_mutex);
        if (r == SFP_READ_OK) {
            sfp_state_set(SFP_STATE_READY);
            send_event(EVENT_SFP, EVENT_SFP_READY);
        }
        else {
            sfp_state_set(SFP_STATE_ERROR);
            send_event(EVENT_SFP, EVENT_SFP_ERROR);
        }

        uint8_t op = 0;
        while (sfp_trg_is_present()) {
            if (enter_sleep == 1)
                break;
            ulTaskNotifyTake(pdTRUE, 500);
            /*if (op == 0) {
                xSemaphoreTake(sfp_mutex, portMAX_DELAY);
                if (sfp_trg_read(0xa2, 96, sizeof(sfp_mon_t), sfp_mon_get()) != SFP_READ_OK) {
                    memset(sfp_mon_get(), 0, sizeof(sfp_mon_t));
                }
                fix_mon(sfp_mon_get());
                xSemaphoreGive(sfp_mutex);
                send_event(EVENT_SFP, EVENT_SFP_DATA);
            }
            else if (op == SFP_REQ_SCAN) {
                sfp_trg_scan();
                send_event(EVENT_SFP, EVENT_SFP_SCAN_DONE);
            }*/
        }
        sfp_hw_set_tx_dis(0);
        sfp_trg_set_power(0);
        sfp_i2c_scan_reset();
        sfp_model_set(SFP_MODEL_GENERIC);
        sfp_state_set(SFP_STATE_NOT_PRESENT);
        send_event(EVENT_SFP, EVENT_SFP_REMOVED);

        if (enter_sleep == 1) {
            if (auto_on)
                HAL_NVIC_EnableIRQ(EXTI2_IRQn);
            else
                HAL_NVIC_DisableIRQ(EXTI2_IRQn);
            enter_sleep = 2;
        }
        else {
            HAL_NVIC_EnableIRQ(EXTI2_IRQn);
        }
        /*
        uint8_t is_present = sfp_hw_is_present();
        if(is_present && !was_present) { //plugged in
            sfp_hw_i2c_disable();
            vTaskDelay(500/portTICK_PERIOD_MS);
            sfp_hw_i2c_enable();
        }
        if(is_present) {
            c='P';
        }
        else
        {
            c='x';
        }
        xQueueSend(test_queue, &c, 0);

        was_present = is_present;
        vTaskDelay(500/portTICK_PERIOD_MS);*/
    }
}

void sfp_hw_set_tx_dis(uint8_t dis)
{
    HAL_GPIO_WritePin(SFP_TX_DIS_GPIO_Port, SFP_TX_DIS_Pin, !!dis);
}

uint8_t sfp_hw_get_tx_dis(void)
{
    return !!(SFP_TX_DIS_GPIO_Port->ODR & SFP_TX_DIS_Pin);
}

uint8_t sfp_hw_read_status(void)
{
    uint8_t st = 0;
    if (HAL_GPIO_ReadPin(SFP_TX_FAULT_GPIO_Port, SFP_TX_FAULT_Pin))
        st |= SFP_STATUS_TX_FAULT;
    if (HAL_GPIO_ReadPin(SFP_LOS_GPIO_Port, SFP_LOS_Pin))
        st |= SFP_STATUS_LOS;
    return st;
}
