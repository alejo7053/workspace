/* This is an example of the Kolban Book, is for connect the ESP32 in mode STA
to an Acces Point*/

#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <tcpip_adapter.h>

esp_err_t event_handler(void *ctx, system_event_t *event)
{
  if (event->event_id == SYSTEM_EVENT_STA_START) {
    printf("STA MODE START 123456789\n");
  }
  if (event->event_id == SYSTEM_EVENT_STA_CONNECTED) {
    printf("STA MODE CONNECTED 123456789\n");
  }
  if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
    printf("Our IP address is " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.ip));
    printf("We have now connected to a station and can do things...\n" );
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
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  wifi_config_t sta_config = {
    .sta = {
      .ssid = "CHAUMO",
      .password = "1094948500",
      .bssid_set = 0
    }
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_connect());

  return 0;
}
