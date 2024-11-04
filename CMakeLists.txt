cmake_minimum_required(VERSION 3.18)

project(oled VERSION 0.1 DESCRIPTION "OLED")

add_compile_options(-Wall)

add_executable(oled_test
    oled_test.cpp
    ssd1306_128x64.cpp
    font_5x7.cpp
    i2c_dev.cpp
    )
