cd /D C:\Users\Cence-Power\Documents\Firmware\projects\t2\receiver\build || exit /b
C:\Users\Cence-Power\.espressifMDF\python_env\idf3.3_py3.8_env\Scripts\python.exe c:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/tools/kconfig_new/confgen.py --kconfig c:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/Kconfig --config C:/Users/Cence-Power/Documents/Firmware/projects/t2/receiver/sdkconfig --env "COMPONENT_KCONFIGS= C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/app_trace/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/aws_iot/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/bt/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/driver/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/efuse/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp32/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_adc_cal/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_event/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_http_client/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_http_server/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_https_ota/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/espcoredump/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/ethernet/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/fatfs/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/freemodbus/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/freertos/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/heap/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/libsodium/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/log/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/lwip/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/mbedtls/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/mdns/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/mqtt/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/nvs_flash/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/openssl/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/pthread/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/spi_flash/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/spiffs/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/tcpip_adapter/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/unity/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/vfs/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/wear_levelling/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/wifi_provisioning/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/wpa_supplicant/Kconfig" --env "COMPONENT_KCONFIGS_PROJBUILD= C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/app_update/Kconfig.projbuild C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/bootloader/Kconfig.projbuild C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esptool_py/Kconfig.projbuild C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/partition_table/Kconfig.projbuild" --env IDF_CMAKE=y --env IDF_TARGET=esp32 --output config C:/Users/Cence-Power/Documents/Firmware/projects/t2/receiver/sdkconfig || exit /b
C:\Users\Cence-Power\.espressifMDF\tools\cmake\3.13.4\bin\cmake.exe -E env "COMPONENT_KCONFIGS= C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/app_trace/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/aws_iot/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/bt/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/driver/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/efuse/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp32/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_adc_cal/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_event/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_http_client/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_http_server/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esp_https_ota/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/espcoredump/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/ethernet/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/fatfs/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/freemodbus/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/freertos/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/heap/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/libsodium/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/log/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/lwip/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/mbedtls/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/mdns/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/mqtt/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/nvs_flash/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/openssl/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/pthread/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/spi_flash/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/spiffs/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/tcpip_adapter/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/unity/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/vfs/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/wear_levelling/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/wifi_provisioning/Kconfig C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/wpa_supplicant/Kconfig" "COMPONENT_KCONFIGS_PROJBUILD= C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/app_update/Kconfig.projbuild C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/bootloader/Kconfig.projbuild C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/esptool_py/Kconfig.projbuild C:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/components/partition_table/Kconfig.projbuild" IDF_CMAKE=y KCONFIG_CONFIG=C:/Users/Cence-Power/Documents/Firmware/projects/t2/receiver/sdkconfig IDF_TARGET=esp32 C:/Users/Cence-Power/.espressifMDF/tools/mconf/v4.6.0.0-idf-20190628/mconf-idf.exe c:/Users/Cence-Power/Documents/Firmware/esp-mdf/esp-idf/Kconfig || exit /b