#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "dht11.h"

void DHT_task(void *pvParameter)
{
   setDHTPin(4);
   printf("Starting DHT measurement!\n");
   while(1)
   {
    vTaskDelay(2000 / portTICK_RATE_MS);
    printf("Temperature %d °C\t",getTemp());
    printf("Humidity %d %%\n",getHumidity());
   }
}

void app_main(void) {

    nvs_flash_init();
    vTaskDelay(2000 / portTICK_RATE_MS);
    xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);

//~/.mos/bin/mos build --local --repo /Users/lemonhall/Downloads/mongoose-os --verbose
}
