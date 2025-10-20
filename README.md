# ESP32 DHT11 Queue Project

Este projeto faz uso de **filas (queues)** para comunicação entre tarefas.  
Uma tarefa produtora lê os valores de **temperatura e umidade** do sensor **DHT11** e envia os dados para a fila.  
Outra tarefa, consumidora, lê os valores e calcula a **média das últimas 5 medições**, exibindo-as no console.

---

## Maiores informações

A comunicação entre as tarefas é feita por meio de uma fila do FreeRTOS, garantindo o envio seguro e sincronizado das medições entre os processos.
O projeto utiliza a ESP-IDF como framework e segue boas práticas de modularização, com um componente separado responsável pela leitura do DHT11.
Após gravar o firmware no ESP32 e abrir o Serial Monitor, o sistema iniciará duas tarefas:

- **Tarefa Produtora (`criarTemperatura`)**: lê a temperatura e a umidade do sensor DHT11 a cada 500 ms e envia os dados para a fila.  
- **Tarefa Consumidora (`consumirTemperatura`)**: lê os dados da fila, armazena os últimos 5 valores e calcula a média da temperatura e da umidade, exibindo os resultados no console.

## Tecnologias utilizadas

- Linguagem C
- Framework ESP-IDF
- FreeRTOS
- Sensor DHT11
- Microcontrolador ESP32
