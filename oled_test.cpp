
#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>

#include "i2c_dev.h"
#include "ssd1306_128x64.h"

#include "font_5x7.h"

const uint8_t i2c_adr = 0x3c;
I2cDev i2c_dev("/dev/i2c-1", i2c_adr, 256);
Ssd1306_128x64 oled(i2c_dev);

static void boxes();
static void stripes();
static void dots();
static void chars();
static void chars_at();
static void fancy();


int main(int argc, char *argv[])
{
    int test_num = -1;
    const char *optstr = "t:";
    int opt;
    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 't':
                test_num = atoi(optarg);
                break;
            default:
                break;
        }
    }

    oled.clear();
    oled.flush();
    oled.on();

    switch (test_num) {
        case 0:
            boxes();
            break;
        case 1:
            stripes();
            break;
        case 2:
            dots();
            break;
        case 3:
            chars();
            break;
        case 4:
            chars_at();
            break;
        case 5:
            fancy();
            break;
        default:
            boxes();
            sleep(1);
            oled.clear();
            stripes();
            sleep(1);
            oled.clear();
            dots();
            sleep(1);
            oled.clear();
            chars();
            sleep(1);
            oled.clear();
            chars_at();
            sleep(1);
            oled.clear();
            fancy();
            break;
    }

    return 0;

} // main


static void boxes()
{
    for (int i = 0; i < 10; i += 2)
        oled.box(i, i, oled.cols - 1 - i, oled.rows - 1 - i);
    oled.flush();
}


static void stripes()
{
    for (int x = 0; x < oled.cols; x += 4) {
        for (int y = 0; y < oled.rows; y++) {
            if (x + y < oled.cols)
                oled.set(x + y, y);
            else
                oled.set(x + y - oled.cols, y);
        }
    }
    oled.flush();
}


static void dots()
{
    oled.set(0, 0);
    oled.set(0, oled.rows - 1);
    oled.set(oled.cols - 1, 0);
    oled.set(oled.cols - 1, oled.rows - 1);
    oled.flush();
}


static void chars()
{
    for (int cc = 0; cc < 16; cc++)
        oled.putc(cc, 0, 0x20 + cc, font_5x7);
    for (int cc = 0; cc < 16; cc++)
        oled.putc(cc, 1, 0x30 + cc, font_5x7);
    for (int cc = 0; cc < 16; cc++)
        oled.putc(cc, 2, 0x40 + cc, font_5x7);
    for (int cc = 0; cc < 16; cc++)
        oled.putc(cc, 3, 0x50 + cc, font_5x7);
    oled.flush();
}


static void chars_at()
{
    // fill white
    for (int x = 0; x < oled.cols; x++)
        for (int y = 0; y < oled.rows; y++)
            oled.set(x, y);

    char c = '0';
    for (int i = 0; i < 20; i++)
        oled.putc_at(i * 6, i, c, font_5x7);

    oled.flush();
}


static void fancy()
{
    oled.box(40, 8, 87, 55);
    oled.putc_at(62, 57, '0', font_5x7);
    oled.putc_at(89, 29, '1', font_5x7);
    oled.putc_at(62,  0, '2', font_5x7);
    oled.putc_at(34, 29, '3', font_5x7);
    oled.flush();
}
