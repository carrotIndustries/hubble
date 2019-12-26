#include "dpy_trg.h"
#include "common/dpy.h"
#include "common/led_hw.h"

static SPI_HandleTypeDef *hspi = 0;

static void Write_Instruction(uint8_t data)
{
    HAL_GPIO_WritePin(OLED_CD_GPIO_Port, OLED_CD_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &data, 1, 1000);
}

static void Write_Data(uint8_t data)
{
    HAL_GPIO_WritePin(OLED_CD_GPIO_Port, OLED_CD_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(hspi, &data, 1, 1000);
}

void Set_Page_Address(unsigned char add)
{
    add = 0xb0 | add;
    Write_Instruction(add);
    return;
}

void Set_Column_Address(unsigned char add)
{
    Write_Instruction((0x10 | (add >> 4)));
    Write_Instruction((0x0f & add));
    return;
}

void Display_Chess(unsigned char value)
{
    unsigned char i, j, k;
    for (i = 0; i < 0x08; i++) {
        Set_Page_Address(i);

        Set_Column_Address(0x00);

        for (j = 0; j < 0x10; j++) {
            for (k = 0; k < 0x04; k++)
                Write_Data(value);
            for (k = 0; k < 0x04; k++)
                Write_Data(~value);
        }
    }
    return;
}

void dpy_trg_init(SPI_HandleTypeDef *h)
{
    hspi = h;
    HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET); // release reset
    HAL_Delay(10);
    Write_Instruction(0xae); //--turn off oled panel

    Write_Instruction(0xd5); //--set display clock divide ratio/oscillator frequency
    Write_Instruction(0x80); //--set divide ratio

    Write_Instruction(0xa8); //--set multiplex ratio(1 to 64)
    Write_Instruction(0x3f); //--1/64 duty

    Write_Instruction(0xd3); //-set display offset
    Write_Instruction(0x00); //-not offset

    Write_Instruction(0x40); // start line


    Write_Instruction(0x8d); //--set Charge Pump enable/disable
    Write_Instruction(0x10); //--set(0x10) disable

    Write_Instruction(0xa1); //--set segment re-map 128 to 0

    Write_Instruction(0xC8); //--Set COM Output Scan Direction 64 to 0

    Write_Instruction(0xda); //--set com pins hardware configuration
    Write_Instruction(0x12);

    Write_Instruction(0x81); //--set contrast control register
    Write_Instruction(0xff);

    Write_Instruction(0xd9); //--set pre-charge period
    Write_Instruction(0x22);

    Write_Instruction(0xdb); //--set vcomh
    Write_Instruction(0x40);

    Write_Instruction(0xa4); // Disable Entire Display On

    Write_Instruction(0xa6); //--set normal display

    HAL_GPIO_WritePin(P12V_OLED_HEN_GPIO_Port, P12V_OLED_HEN_Pin, GPIO_PIN_SET);

    HAL_Delay(200);

    Write_Instruction(0xaf);

    Display_Chess(0x0f);
}

void dpy_trg_flush(void)
{
    const uint8_t *fb = dpy_get_fb();
    for (uint8_t page = 0; page < 8; page++) {
        Set_Page_Address(page);
        Set_Column_Address(0x00);
        for (uint8_t col = 0; col < 128; col++) {
            Write_Data(fb[page * 128 + col]);
        }
    }
}

void dpy_trg_enter_sleep(void) {
	Write_Instruction(0xae);
	HAL_GPIO_WritePin(P12V_OLED_HEN_GPIO_Port, P12V_OLED_HEN_Pin, GPIO_PIN_RESET);
}

void dpy_trg_exit_sleep(void) {
	HAL_GPIO_WritePin(P12V_OLED_HEN_GPIO_Port, P12V_OLED_HEN_Pin, GPIO_PIN_SET);
	led_hw_set(0xffff);
	vTaskDelay(5);
	led_hw_set(0);
	vTaskDelay(10);
	Write_Instruction(0xaf);
}

static uint8_t brightness = 15;

void dpy_hw_set_brightness(uint8_t b) {
	brightness = b;
	if(b == 15) {
		b = 0xff;
	}
	else {
		b *= 16;
	}
	Write_Instruction(0x81); //--set contrast control register
    Write_Instruction(b);
}

uint8_t dpy_hw_get_brightness(void) {
	return brightness;
}
