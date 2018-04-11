#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/semphr.h"

//creo el manejador para el semaforo como variable global
SemaphoreHandle_t xSemaphore = NULL;

void IRAM_ATTR detector(void *arg) {
	//da el semaforo para que quede libre para la terea pulsador
	xSemaphoreGiveFromISR(xSemaphore, NULL);
}

void init_GPIO(){
	//Iniciar GPIO
	gpio_pad_select_gpio(GPIO_NUM_32); // seleccion PIN
	gpio_pad_select_gpio(GPIO_NUM_17); // seleccion PIN
	gpio_set_direction(GPIO_NUM_32, GPIO_MODE_INPUT); //Pin como entrada
	gpio_set_direction(GPIO_NUM_17, GPIO_MODE_OUTPUT); //pin como salida
	gpio_set_pull_mode(GPIO_NUM_32, GPIO_PULLDOWN_ONLY);
	gpio_install_isr_service(0); //Config por defecto ISR
	gpio_isr_handler_add(GPIO_NUM_32, detector, NULL); //manejador de interrupcion
	gpio_set_intr_type(GPIO_NUM_32, GPIO_INTR_POSEDGE); //interrupcion Rising
}

void task_pulsador(void* arg) {

	while(1) {

		// Espero por la notificacion de la ISR
		if(xSemaphoreTake(xSemaphore,portMAX_DELAY) == pdTRUE) {
			//printf("X!\n");
			vTaskDelay(7 / portTICK_PERIOD_MS);
			gpio_set_level(GPIO_NUM_17, 1);
		}
		//vTaskDelay(1 / portTICK_PERIOD_MS);
		//gpio_set_level(GPIO_NUM_17,0);
	}
}

void app_main(){
	init_GPIO(); //Inicia GPIO 32 con ISR

	//se crea el semaforo binario
	xSemaphore = xSemaphoreCreateBinary();

	//creo la tarea task pulsador
	xTaskCreate(task_pulsador, "task_pulsador", 2048, NULL, 5, NULL);
}
