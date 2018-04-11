/* This is an example of the Kolban Book, is for connect the ESP32 in mode AP*/

#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <esp_err.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <tcpip_adapter.h>

esp_err_t event_handler(void *ctx, system_event_t *event)
{
  if(event->event_id == SYSTEM_EVENT_AP_STACONNECTED) {
    printf("UNO MAS\n");
  }
  if(event->event_id == SYSTEM_EVENT_AP_STADISCONNECTED) {
    printf("UNO MENOS\n");
  }
  return ESP_OK;
}

int app_main(void)
{
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  wifi_config_t ap_config = {
    .ap = {
      .ssid = "TestESP32",
      .password = "password",
      .channel = 1,
      .authmode=WIFI_AUTH_WPA2_PSK,
      .ssid_hidden=0,
      .max_connection=4
    }
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  return 0;
}
