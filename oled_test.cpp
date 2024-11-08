
#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
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
static void chars2();
static void chars_at();
static void fancy();
static void fancy2();
static void fills();


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
            oled.on();
            break;
        case 1:
            boxes();
            break;
        case 2:
            stripes();
            break;
        case 3:
            dots();
            break;
        case 4:
            chars();
            break;
        case 5:
            chars2();
            break;
        case 6:
            chars_at();
            break;
        case 7:
            fancy();
            break;
        case 8:
            fancy2();
            break;
        case 9:
            fills();
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
            chars2();
            sleep(1);
            oled.clear();
            chars_at();
            sleep(1);
            oled.clear();
            fancy();
            sleep(1);
            oled.clear();
            fancy2();
            sleep(1);
            oled.clear();
            fills();
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
    oled.puts(0, 0, " !\"#$%&'()*+,-./", font_5x7);
    oled.puts(0, 1, "0123456789:;<=>?", font_5x7);
    oled.puts(0, 2, "@ABCDEFGHIJKLMNO", font_5x7);
    oled.puts(0, 3, "PQRSTUVWXYZ[\\]^_", font_5x7);
    oled.flush();
}


static void chars2()
{
    oled.puts2(0, 0, "0123456789", font_5x7);
    oled.puts2(0, 2, "ABCDEFGHIJ", font_5x7);
    oled.puts2(0, 4, "KLMNOPQRST", font_5x7);
    oled.puts2(0, 6, "!@#$%^&*()", font_5x7);
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
    oled.putc_at(62, 57, '0', font_5x7); // bottom
    oled.putc_at(89, 29, '1', font_5x7); // right
    oled.putc_at(62,  0, '2', font_5x7); // top
    oled.putc_at(34, 29, '3', font_5x7); // left
    oled.flush();
}


static void fancy2()
{
    //       x1  y1  x2  y2
    oled.box(48, 16, 81, 45);
    //        col row char
    oled.putc2(10, 6, '4', font_5x7); // bottom
    oled.putc2(14, 3, '5', font_5x7); // right
    oled.putc2(10, 0, '6', font_5x7); // top
    oled.putc2( 6, 3, '7', font_5x7); // left
    oled.flush();
}


static void fills()
{
    //        x1  y1   x2  y2
    oled.fill( 0,  0,   0,  0); // dot in upper left corner
    oled.fill( 2,  2,   3,  3); // 2x2
    oled.fill( 5,  0, 127, 63); // 2x2
    oled.flush();
}
