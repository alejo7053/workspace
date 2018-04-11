/* HTTP Request Command*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "cmd_decl.h"

static const char *TAG = "My_example";

static struct{
    struct arg_str *server;
    struct arg_str *url;
    struct arg_end *end;
} http_args;

static int http_req(int argc, char** argv)
{

    int nerrors = arg_parse(argc, argv, (void**) &http_args);
    if (nerrors != 0) {
      arg_print_errors(stderr, http_args.end, argv[0]);
      return 1;
    }

//Buscar STRING para manejar la cadena de REQUEST

    /*char REQUEST[1000] = "GET ";
    strcat( REQUEST, http_args.url->sval[0]);
    strcat( REQUEST, " HTTP/1.0\r\nHost: ");
    strcat( REQUEST, http_args.server->sval[0]);
    strcat( REQUEST, "\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n");*/

    const char *GET_FORMAT =
        "GET %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "User-Agent: esp-idf/1.0 esp32\r\n\r\n";

    char *REQUEST = NULL;
    asprintf(&REQUEST, GET_FORMAT, http_args.url->sval[0],
       http_args.server->sval[0]);

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];


        int err = getaddrinfo(http_args.server->sval[0], "80", &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);

        free(REQUEST);

    return 0;
}

void register_http()
{
  http_args.server = arg_str0(NULL, NULL, "<server>", "Server, Ex: patorjk.com");
  http_args.url = arg_str1(NULL, NULL, "<url>", "URL, Ex: \"/software/taag/#p=display&f=3D Diagonal&t=ESP 32\"");
  http_args.end = arg_end(3);

  const esp_console_cmd_t cmd = {
      .command = "http",
      .help = "HTTP Request",
      .hint = NULL,
      .func = &http_req,
      .argtable = &http_args
  };

  ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
