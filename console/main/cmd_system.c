/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "argtable3/argtable3.h"
#include "cmd_decl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_cntl_reg.h"
#include "sdkconfig.h"

#ifdef CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
#define WITH_TASKS_INFO 1
#endif

static void register_free();
static void register_restart();
static void register_deep_sleep();
#if WITH_TASKS_INFO
static void register_tasks();
#endif

void register_system()
{
    register_free();
    register_restart();
    register_deep_sleep();
#if WITH_TASKS_INFO
    register_tasks();
#endif
}

/** 'restart' command restarts the program */

static int restart(int argc, char** argv)
{
    ESP_LOGI(__func__, "Restarting");
    esp_restart();
}

static void register_restart()
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Restart the program",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'free' command prints available heap memory */

static int free_mem(int argc, char** argv)
{
    printf("%d\n", esp_get_free_heap_size());
    return 0;
}

static void register_free()
{
    const esp_console_cmd_t cmd = {
        .command = "free",
        .help = "Get the total size of heap memory available",
        .hint = NULL,
        .func = &free_mem,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'tasks' command prints the list of tasks and related information */
#if WITH_TASKS_INFO

static int tasks_info(int argc, char** argv)
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char* task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        ESP_LOGE(__func__, "failed to allocate buffer for vTaskList output");
        return 1;
    }
    fputs("Task Name\tStatus\tPrio\tHWM\tTask Number\n", stdout);
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
    return 0;
}

static void register_tasks()
{
    const esp_console_cmd_t cmd = {
        .command = "tasks",
        .help = "Get information about running tasks",
        .hint = NULL,
        .func = &tasks_info,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

#endif // WITH_TASKS_INFO

/** 'deep_sleep' command puts the chip into deep sleep mode */

static struct {
    struct arg_int *wakeup_time;
    struct arg_int *wakeup_gpio_num;
    struct arg_int *wakeup_gpio_level;
    struct arg_end *end;
} deep_sleep_args;


static int deep_sleep(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &deep_sleep_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, deep_sleep_args.end, argv[0]);
        return 1;
    }
    if (deep_sleep_args.wakeup_time->count) {
        uint64_t timeout = 1000ULL * deep_sleep_args.wakeup_time->ival[0];
        ESP_LOGI(__func__, "Enabling timer wakeup, timeout=%lluus", timeout);
        ESP_ERROR_CHECK( esp_sleep_enable_timer_wakeup(timeout) );
    }
    if (deep_sleep_args.wakeup_gpio_num->count) {
        int io_num = deep_sleep_args.wakeup_gpio_num->ival[0];
        if (!rtc_gpio_is_valid_gpio(io_num)) {
            ESP_LOGE(__func__, "GPIO %d is not an RTC IO", io_num);
            return 1;
        }
        int level = 0;
        if (deep_sleep_args.wakeup_gpio_level->count) {
            level = deep_sleep_args.wakeup_gpio_level->ival[0];
            if (level != 0 && level != 1) {
                ESP_LOGE(__func__, "Invalid wakeup level: %d", level);
                return 1;
            }
        }
        ESP_LOGI(__func__, "Enabling wakeup on GPIO%d, wakeup on %s level",
                io_num, level ? "HIGH" : "LOW");

        ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(1ULL << io_num, level) );
    }
    rtc_gpio_isolate(GPIO_NUM_12);
    esp_deep_sleep_start();
}

static void register_deep_sleep()
{
    deep_sleep_args.wakeup_time =
            arg_int0("t", "time", "<t>", "Wake up time, ms");
    deep_sleep_args.wakeup_gpio_num =
            arg_int0(NULL, "io", "<n>",
                     "If specified, wakeup using GPIO with given number");
    deep_sleep_args.wakeup_gpio_level =
            arg_int0(NULL, "io_level", "<0|1>", "GPIO level to trigger wakeup");
    deep_sleep_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "deep_sleep",
        .help = "Enter deep sleep mode. "
                "Two wakeup modes are supported: timer and GPIO. "
                "If no wakeup option is specified, will sleep indefinitely.",
        .hint = NULL,
        .func = &deep_sleep,
        .argtable = &deep_sleep_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
