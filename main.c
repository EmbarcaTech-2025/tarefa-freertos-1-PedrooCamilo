#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BUZZER_PIN 21
#define BTN_A_PIN 5
#define BTN_B_PIN 6

TaskHandle_t led_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;

void led_rgb_task(void *pvParameters) {
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    while (true) {
        // vermelho
        gpio_put(LED_R_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_R_PIN, 0);
        // verde
        gpio_put(LED_G_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_G_PIN, 0);

        // azul
        gpio_put(LED_B_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_B_PIN, 0);
    }
}

void buzzer_task(void *pvParameters) {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    while (true) {
        gpio_put(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50)); // bip de 50ms
        gpio_put(BUZZER_PIN, 0); // Desliga
        vTaskDelay(pdMS_TO_TICKS(950)); // espera o resto do tempo
    }
}

void button_monitor_task(void *pvParameters) {
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN); 

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);

    bool led_suspended = false;
    bool buzzer_suspended = false;

    while (true) {
      // lógica pro botão A
        //botão pressionado
        if (!gpio_get(BTN_A_PIN)) { 
            if (!led_suspended) {
                vTaskSuspend(led_task_handle);
                led_suspended = true;
            }
        } else { // botão ñ pressionado
            if (led_suspended) {
                vTaskResume(led_task_handle);
                led_suspended = false;
            }
        }
      // lógica pro botão B
        // botão pressionado
        if (!gpio_get(BTN_B_PIN)) { 
            if (!buzzer_suspended) {
                vTaskSuspend(buzzer_task_handle);
                buzzer_suspended = true;
            }
        } else { // botão ñ pressionado
            if (buzzer_suspended) {
                vTaskResume(buzzer_task_handle);
                buzzer_suspended = false;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // verifica os botões a cada 100ms
    }
}

int main() {
    stdio_init_all();
    // criação das tarefas
    xTaskCreate(led_rgb_task, "LED_Task", 256, NULL, 1, &led_task_handle);
    xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 1, &buzzer_task_handle);
    xTaskCreate(button_monitor_task, "Button_Task", 256, NULL, 2, NULL); 

    vTaskStartScheduler();

    while (1) {};
}