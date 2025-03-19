# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(SmartBox_firmware)

target_include_directories(app PRIVATE 
    SW/DRV/Drv_Gpio/src
    SW/DRV/Drv_Gpio/cfg
    SW/SRV/
    SW/SRV/Lowpass_Filter\Src
    SW/HAL/Hal_Led/src
    SW/HAL/Hal_Led/cfg
    SW/HAL/Hal_Ipc/src
    SW/HAL/Hal_Ipc/cfg
    SW/APP/App_Led/src
    SW/APP/App_Led/cfg
    SW/APP/App_Ipc/src
    SW/APP/App_Ipc/cfg
)

target_sources(app PRIVATE 
    SW/main.c 
    SW/DRV/Drv_Gpio/src/drv_gpio.c
    SW/SRV/Lowpass_Filter\Src\lowpass_filter.c
    SW/HAL/Hal_Led/src/hal_led.c
    SW/HAL/Hal_Ipc/src/hal_ipc.c
    SW/APP/App_Led/src/app_led.c
    SW/APP/App_Ipc/src/app_ipc.c)
