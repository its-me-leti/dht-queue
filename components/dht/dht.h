#ifndef DHT_H
#define DHT_H

#include "driver/gpio.h"
#include "esp_err.h"

#define DHT_TYPE_DHT11 11
#define DHT_TYPE_DHT22 22

esp_err_t dht_read_float_data(int type, gpio_num_t pin, float *humidity, float *temperature);

#endif
