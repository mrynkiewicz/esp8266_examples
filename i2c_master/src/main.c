#include "osapi.h"
#include "gpio.h"
#include "user_interface.h"
#include "driver/i2c_master.h"


#define LED_PIN 2
#define I2C_ADDRESS 4

#define SUCCESS 0
#define ADDR_ERROR 1
#define DATA_ERROR 2

static os_timer_t blinky_timer;
static void blinky_timer_handler(void *prv);


uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}


uint8_t ICACHE_FLASH_ATTR i2cwrite(uint8_t address, const char* string, uint8_t size)
{
    i2c_master_start();
    i2c_master_writeByte(address);
    if(!i2c_master_checkAck()) return ADDR_ERROR;

    for(int i = 0; i < size; i++) {
        i2c_master_writeByte(string[i]);
        if(!i2c_master_checkAck()) return DATA_ERROR;
    }

    i2c_master_stop();
    return SUCCESS;
}

void ICACHE_FLASH_ATTR
user_init(void) {
    
    gpio_init();
    i2c_master_gpio_init();
    gpio_output_set(0, 0, (1 << LED_PIN), 0);
    uart_div_modify(0, UART_CLK_FREQ / 115200);

    os_printf("\n\nSDK version:%s\n\n", system_get_sdk_version());

    // delay to let I2C pin settled
    // without this delay odd traces
    // were observed on 
    os_delay_us(10000);

    const char* msg = "Hello world";
    i2cwrite(I2C_ADDRESS << 1, msg, strlen(msg));
    os_timer_setfn(&blinky_timer, (os_timer_func_t *)blinky_timer_handler, NULL);
    os_timer_arm(&blinky_timer, 1000, 1);

    os_printf("This is it\n");


}

void blinky_timer_handler(void *prv)
{
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << LED_PIN))
    {
        gpio_output_set(0, (1 << LED_PIN), 0, 0);
    }
    else
    {
        gpio_output_set((1 << LED_PIN), 0, 0, 0);
    }
}
