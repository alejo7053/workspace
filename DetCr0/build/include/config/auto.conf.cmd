deps_config := \
	/root/esp32/esp-idf/components/app_trace/Kconfig \
	/root/esp32/esp-idf/components/aws_iot/Kconfig \
	/root/esp32/esp-idf/components/bt/Kconfig \
	/root/esp32/esp-idf/components/esp32/Kconfig \
	/root/esp32/esp-idf/components/ethernet/Kconfig \
	/root/esp32/esp-idf/components/fatfs/Kconfig \
	/root/esp32/esp-idf/components/freertos/Kconfig \
	/root/esp32/esp-idf/components/heap/Kconfig \
	/root/esp32/esp-idf/components/libsodium/Kconfig \
	/root/esp32/esp-idf/components/log/Kconfig \
	/root/esp32/esp-idf/components/lwip/Kconfig \
	/root/esp32/esp-idf/components/mbedtls/Kconfig \
	/root/esp32/esp-idf/components/openssl/Kconfig \
	/root/esp32/esp-idf/components/pthread/Kconfig \
	/root/esp32/esp-idf/components/spi_flash/Kconfig \
	/root/esp32/esp-idf/components/spiffs/Kconfig \
	/root/esp32/esp-idf/components/tcpip_adapter/Kconfig \
	/root/esp32/esp-idf/components/wear_levelling/Kconfig \
	/root/esp32/esp-idf/components/bootloader/Kconfig.projbuild \
	/root/esp32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/root/esp32/esp-idf/components/partition_table/Kconfig.projbuild \
	/root/esp32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
