#include "dht.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "esp_log.h"

#define TAG "DHT"

static int esperar_nivel(gpio_num_t pin, int nivel, uint32_t timeout_us)
{
    uint32_t count = 0;
    while (gpio_get_level(pin) == nivel)
    {
        if (++count > timeout_us)
            return -1;
        esp_rom_delay_us(2);
    }
    return count;
}

esp_err_t dht_read_float_data(int type, gpio_num_t pin, float *humidity, float *temperature)
{
    uint8_t data[5] = {0};

    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    esp_rom_delay_us(type == DHT_TYPE_DHT11 ? 18000 : 1000);
    gpio_set_level(pin, 1);
    esp_rom_delay_us(40);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    if (esperar_nivel(pin, 1, 80) == -1)
        return ESP_FAIL;
    if (esperar_nivel(pin, 0, 80) == -1)
        return ESP_FAIL;

    for (int i = 0; i < 40; i++)
    {
        if (esperar_nivel(pin, 1, 50) == -1)
            return ESP_FAIL;
        int len = esperar_nivel(pin, 0, 70);
        if (len == -1)
            return ESP_FAIL;

        data[i / 8] <<= 1;
        if (len > 28)
            data[i / 8] |= 1;
    }

    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if ((checksum & 0xFF) != data[4])
        return ESP_FAIL;

    if (type == DHT_TYPE_DHT11)
    {
        *humidity = (float)data[0];      // Parte inteira da umidade
        *temperature = (float)data[2];   // Parte inteira da temperatura
    }
    else // DHT22
    {
        *humidity = ((data[0] << 8) | data[1]) * 0.1f;
        *temperature = ((data[2] & 0x7F) << 8 | data[3]) * 0.1f;
        if (data[2] & 0x80)
            *temperature = -*temperature;
    }

    return ESP_OK;
}
