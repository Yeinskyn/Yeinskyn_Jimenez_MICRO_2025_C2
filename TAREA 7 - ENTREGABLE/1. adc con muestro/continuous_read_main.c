#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/timers.h"
#include "driver/adc.h"
#include "esp_timer.h" //sirve para el ADC con muestreo

#define led1 2
#define ledR 33
#define ledG 25
#define ledB 26
// 1s/2400 = 416us (microsegundos)
#define SAMPLE_PERIOD_US 416 // 416 significa el tiempo necesario por microsegundo para obtener 2400 por segundo

uint8_t led_level = 0;
static const char *tag = "Main";

int adc_val = 0;
esp_timer_handle_t adc_timer; // Es el TIMER de alta resolución para que se pueda hacer el muestreo ADC

esp_err_t init_led(void);
esp_err_t blink_led(void);
esp_err_t set_timer(void);
esp_err_t set_adc(void);
esp_err_t set_highres_timer(void); // muestreo

void vTimerCallback(void *arg) // Esto es necesario porque esp_timer usa una función callback con la forma: void (esp_timer_cb_t)(void arg)
{
    blink_led();
    adc_val = adc1_get_raw(ADC1_CHANNEL_4);
    int adc_case = adc_val / 1000; // maximo es 4094
    ESP_LOGI(tag, "ADC VAL: %i", adc_val);

    switch (adc_case)
    {

    case 0:
        gpio_set_level(ledR, 0);
        gpio_set_level(ledG, 0);
        gpio_set_level(ledB, 0);
        break;

    case 1:
        gpio_set_level(ledR, 1);
        gpio_set_level(ledG, 0);
        gpio_set_level(ledB, 0);
        break;

    case 2:
        gpio_set_level(ledR, 1);
        gpio_set_level(ledG, 1);
        gpio_set_level(ledB, 0);
        break;

    case 3:
    case 4:
        gpio_set_level(ledR, 1);
        gpio_set_level(ledG, 1);
        gpio_set_level(ledB, 1);
        break;

    default:
        break;
    }
}

void app_main(void)
{
    init_led();
    set_adc();           // Esto inicializa el ADC correctamente
    set_highres_timer(); // muestreo
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(tag, "Programa iniciado correctamente");
}

esp_err_t init_led(void)
{
    gpio_reset_pin(led1);
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledB);
    gpio_set_direction(ledB, GPIO_MODE_OUTPUT);

    return ESP_OK;
}

esp_err_t blink_led(void)
{
    led_level = !led_level;
    gpio_set_level(led1, led_level);
    return ESP_OK;
}

esp_err_t set_adc(void)
{
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
    return ESP_OK;
}

// NUEVA función para configurar el esp_timer
esp_err_t set_highres_timer(void)
{
    const esp_timer_create_args_t adc_timer_args = {
        .callback = &vTimerCallback,
        .name = "adc_sample_timer"};

    ESP_ERROR_CHECK(esp_timer_create(&adc_timer_args, &adc_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(adc_timer, SAMPLE_PERIOD_US)); // 416 us

    ESP_LOGI(tag, "High-res ADC timer started (416 us interval)");
    return ESP_OK;
}