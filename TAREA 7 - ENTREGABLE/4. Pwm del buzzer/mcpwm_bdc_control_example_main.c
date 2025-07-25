#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define BUZZER_GPIO 18 // Cambia este pin según tu conexión
int duty_cycle_percent = 50;

void app_main(void)
{
    // Calcular los ticks del duty (resolución de 10 bits = 1023)
    uint32_t duty_ticks = (duty_cycle_percent * 1023) / 100;

    // Configurar temporizador PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000, // 1 kHz para buzzer pasivo
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    // Configurar canal PWM
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BUZZER_GPIO,
        .duty = 512, // 50% duty (1023 / 2)
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);

    printf("\nPWM iniciado a 50%% para el buzzer en el pin %d\n\n", BUZZER_GPIO);

    // Mantener el buzzer sonando
    while (1)
    {
        printf("PWM sigue activo en GPIO%d con %d%% de duty\n", BUZZER_GPIO, duty_cycle_percent);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Pausa de 2 segundos
    } 
}