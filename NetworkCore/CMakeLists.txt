# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(SmartBox_firmware_network_core)

target_include_directories(app PRIVATE 
    SW/SRV/
    SW/HAL/Hal_Ble/src
    SW/HAL/Hal_Ble/cfg
    SW/HAL/Hal_Ipc/src
    SW/HAL/Hal_Ipc/cfg
)

target_sources(app PRIVATE 
    SW/main.c 
    SW/HAL/Hal_Ble/src/hal_ble.c
    SW/HAL/Hal_Ipc/src/hal_ipc.c
    SW/HAL/Hal_Ipc/cfg/hal_ipc_decode.c)