#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED0 GPIO_NUM_19
#define LED1 GPIO_NUM_45
#define LED2 GPIO_NUM_39
#define LED3 GPIO_NUM_1

#define BUTTON_A GPIO_NUM_46
#define BUTTON_B GPIO_NUM_5

int contador = 0;
int incremento = 1;

#define DEBOUNCE_MS 50  // tempo de debounce em milissegundos

// Armazenando os tempos de última leitura dos botões
TickType_t ultima_troca_btn_a = 0;
TickType_t ultima_troca_btn_b = 0;

// Função para atualizar os LEDs com valor binário
void atualizar_leds(int valor) {
    gpio_set_level(LED0, valor & 1);
    gpio_set_level(LED1, (valor >> 1) & 1);
    gpio_set_level(LED2, (valor >> 2) & 1);
    gpio_set_level(LED3, (valor >> 3) & 1);
}

void app_main(void) {
    // Configurar LEDs como saída
    gpio_set_direction(LED0, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);


    gpio_config_t btn_conf = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << BUTTON_A) | (1ULL << BUTTON_B)
    };
    gpio_config(&btn_conf);

    int estado_btn_a_ant = gpio_get_level(BUTTON_A);
    int estado_btn_b_ant = gpio_get_level(BUTTON_B);

    while (1) {
        int estado_btn_a = gpio_get_level(BUTTON_A);
        int estado_btn_b = gpio_get_level(BUTTON_B);
        TickType_t agora = xTaskGetTickCount();

        // Botão A - Incrementa o contador
        if (estado_btn_a == 0 && estado_btn_a_ant == 1 &&
            (agora - ultima_troca_btn_a > pdMS_TO_TICKS(DEBOUNCE_MS))) {
            
            contador = (contador + incremento) % 16;  
            atualizar_leds(contador);
            ultima_troca_btn_a = agora;  // Atualiza o tempo da última troca do botão A
        }

        // Botão B - Alterna a unidade de incremento
        if (estado_btn_b == 0 && estado_btn_b_ant == 1 &&
            (agora - ultima_troca_btn_b > pdMS_TO_TICKS(DEBOUNCE_MS))) {
            
            incremento = (incremento == 1) ? 2 : 1;  
            ultima_troca_btn_b = agora;  // Atualiza o tempo da última troca do botão B
        }

        estado_btn_a_ant = estado_btn_a;
        estado_btn_b_ant = estado_btn_b;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
