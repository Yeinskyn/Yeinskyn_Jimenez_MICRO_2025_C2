#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define MOTOR_PWM_GPIO 18 // Cambia si usas otro pin

void app_main(void)
{
    // Configurar el temporizador del PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT, // 0-1023
        .freq_hz = 1000,                      // Frecuencia PWM
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    // Configurar canal PWM
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = MOTOR_PWM_GPIO,
        .duty = 0, // Inicialmente apagado
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);

    // Bucle para variar la velocidad del motor
    while (1)
    {
        for (int duty = 0; duty <= 1023; duty += 100)
        {
            ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
            ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
            printf("PWM duty: %d\n", duty);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        for (int duty = 1023; duty >= 0; duty -= 100)
        {
            ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
            ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
            printf("PWM duty: %d\n", duty);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}