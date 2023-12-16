cmake_minimum_required(VERSION 3.13)

#include build functions from SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

#
project(picopixelz C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#
set(PICO_BOARD pico_w)

add_compile_options(
	-Wall
	-Werror
)

pico_sdk_init()

#add_subdirectory(lib/FatFs_SPI build)
#add_subdirectory(lib/ly)

add_subdirectory(no-OS-FatFS/src)

# Flex and Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(CONFIG_LEXER
    ./ly/src/config_lexer.l
    "${CMAKE_CURRENT_BINARY_DIR}/config_lexer.c"
    COMPILE_FLAGS "--header-file=lex.yy.h"
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/config_lexer.h)


bison_target(CONFIG_PARSER
    ./ly/src/config_parser.y
    "${CMAKE_CURRENT_BINARY_DIR}/config_parser.c"
    COMPILE_FLAGS "--warnings=all --debug --verbose -d"
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/config_parser.h)
 
add_flex_bison_dependency(CONFIG_LEXER CONFIG_PARSER)

#
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_executable(picopixelz
    ppixelz.c
    ws2812.c
    artnet.c
    web.c
    hw_config.c
    config.c
    ${CMAKE_CURRENT_BINARY_DIR}/config_lexer.c
    ${CMAKE_CURRENT_BINARY_DIR}/config_parser.c
)

#
pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

pico_enable_stdio_usb(picopixelz 1)
pico_enable_stdio_uart(picopixelz 1)

pico_add_extra_outputs(picopixelz)
#
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}  
  	pico_stdlib 
	pico_util
	FatFs_SPI
	hardware_spi
	hardware_clocks
	# pico_cyw43_arch_lwip_poll
	pico_cyw43_arch_lwip_threadsafe_background
	pico_lwip_http
)


