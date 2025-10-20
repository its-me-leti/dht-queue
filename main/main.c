#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "dht.h"

#define DHT_PIN GPIO_NUM_4 // Pino de dados do DHT11

typedef struct
{
    float temperatura;
    float umidade;
} leitura_dht_t;

QueueHandle_t fila_dht;

// ======= TAREFA PRODUTORA =======
void tarefa_produtora(void *pvParameters)
{
    leitura_dht_t leitura;
    esp_err_t res;

    while (1)
    {
        res = dht_read_float_data(DHT_TYPE_DHT11, DHT_PIN, &leitura.umidade, &leitura.temperatura);

        if (res == ESP_OK)
        {
            if (xQueueSend(fila_dht, &leitura, 0) == pdTRUE)
            {
                printf("Produtora: Temp = %.1f°C, Umid = %.1f%% enviada à fila\n",
                       leitura.temperatura, leitura.umidade);
            }
            else
            {
                printf("Fila cheia, descartando leitura\n");
            }
        }
        else
        {
            printf("Falha ao ler DHT11\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Leitura a cada 2s
    }
}

// ======= TAREFA CONSUMIDORA =======
void tarefa_consumidora(void *pvParameters)
{
    leitura_dht_t buffer[5];
    int count = 0;
    float somaTemp = 0, somaUmid = 0;
    leitura_dht_t leitura;

    while (1)
    {
        if (xQueueReceive(fila_dht, &leitura, portMAX_DELAY))
        {
            buffer[count % 5] = leitura;
            count++;

            if (count >= 5)
            {
                somaTemp = 0;
                somaUmid = 0;
                for (int i = 0; i < 5; i++)
                {
                    somaTemp += buffer[i].temperatura;
                    somaUmid += buffer[i].umidade;
                }

                float mediaTemp = somaTemp / 5.0;
                float mediaUmid = somaUmid / 5.0;

                printf("Consumidora: Média das últimas 5 leituras -> Temp = %.1f°C | Umid = %.1f%%\n",
                       mediaTemp, mediaUmid);
            }
        }
    }
}

// ======= APP_MAIN =======
void app_main(void)
{
    printf("Iniciando leitura do DHT11 com FILA...\n");

    fila_dht = xQueueCreate(5, sizeof(leitura_dht_t));
    if (fila_dht == NULL)
    {
        printf("Erro ao criar a fila!\n");
        return;
    }

    xTaskCreate(tarefa_produtora, "Produtora", 4096, NULL, 2, NULL);
    xTaskCreate(tarefa_consumidora, "Consumidora", 4096, NULL, 1, NULL);
}
