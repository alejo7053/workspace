/*GPIO Command*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "cmd_decl.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

static struct{
    struct arg_int *pin_number;
    struct arg_str *pin_mode;
    struct arg_int *pin_state;
    struct arg_end *end;
} GPIO_args;

static int GPIO_req(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &GPIO_args);
    if (nerrors != 0) {
      arg_print_errors(stderr, GPIO_args.end, argv[0]);
      return 1;
    }

    gpio_pad_select_gpio(GPIO_args.pin_number->ival[0]);

    printf("%d,%s,%d\n", GPIO_args.pin_number->ival[0], GPIO_args.pin_mode->sval[0], GPIO_args.pin_state->ival[0]);
    int a=0;
    a=strcmp(GPIO_args.pin_mode->sval[0],'o');
    printf("%d\n", a);
    /*if (strcmp(GPIO_args.pin_mode->sval[0],"o")){
        gpio_set_direction(GPIO_args.pin_number->ival[0], GPIO_MODE_OUTPUT);
        gpio_set_level(GPIO_args.pin_number->ival[0], GPIO_args.pin_state->ival[0]);
        printf("hola");
        //vTaskDelay(4000 / portTICK_PERIOD_MS);
      }*/

    if (GPIO_args.pin_mode->sval[0] == 'i'){
        gpio_set_direction(GPIO_args.pin_number->ival[0], GPIO_MODE_INPUT);
      }

    return 0;
}

void register_GPIO()
{
    GPIO_args.pin_number = arg_int0(NULL, "n", "<n>", "Pin number");
    GPIO_args.pin_mode = arg_str0(NULL, NULL, "<i/o>", "input/output");
    GPIO_args.pin_state = arg_int0(NULL, "s", "<s>", "High = 1, Low = 0");
    GPIO_args.end = arg_end(2);


    const esp_console_cmd_t cmd = {
        .command = "pin",
        .help = "Select pin as output or input",
        .hint = NULL,
        .func = &GPIO_req,
        .argtable = &GPIO_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
