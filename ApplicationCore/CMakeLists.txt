# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(SmartBox_firmware)

target_include_directories(app PRIVATE 
    SW/SRV/
    SW/HAL/Hal_Led/src
    SW/HAL/Hal_Led/cfg
    SW/APP/App_Led/src
    SW/APP/App_Led/cfg
    SW/APP/App_Ble/src
    SW/APP/App_Ble/cfg
)

target_sources(app PRIVATE 
    SW/main.c 
    SW/HAL/Hal_Led/src/hal_led.c
    SW/APP/App_Led/src/app_led.c
    SW/APP/App_Ble/src/app_ble.c)
