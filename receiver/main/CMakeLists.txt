set(COMPONENT_SRCS "handle_commands.c" "ccp_util.c" "app_main.c" "SPI_comm.c" "gpio.c" "handshake.c" "action.c" "handle_commands.c" "query.c" "control.c" "logging.c" "I2C_comm.c" "src/atecc508a.c" "src/atecc508a_comm.c" "src/atecc508a_conf.c" "src/atecc508a_crc.c" "src/atecc508a_pwr.c" "src/atecc508a_sec.c" "src/atecc508a_util.c" "temp.c")
set(COMPONENT_ADD_INCLUDEDIRS "." "include")

register_component()
