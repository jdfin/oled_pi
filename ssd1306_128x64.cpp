
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <iostream>
#include "i2c_dev.h"
#include "ssd1306_128x64.h"

using std::cout;
using std::endl;
using std::invalid_argument;


Ssd1306_128x64::Ssd1306_128x64(I2cDev& i2c_dev) :
    _i2c_dev(i2c_dev)
{
    memset(_image, 0, sizeof(_image));

    off(); // display off

    write_cmd(0xa8, 0x3f);  // mux ratio 64 (reset value)

    write_cmd(0x8d, 0x14);  // charge pump regulator enabled

    // flip vertically or not ([0xc0], 0xc8)
    if (flip_v)
        write_cmd(0xc8);
    else
        write_cmd(0xc0);

    // flip horizontally or not ([0xa0], 0xa1)
    if (flip_h)
        write_cmd(0xa1);
    else
        write_cmd(0xa0);

    write_cmd(0xda, 0x12);  // sequential com pins, no l/r remap
                            // (0xda,0x02, [0xda/0x12], 0xda/0x22, 0xda/0x32)

    write_cmd(0xd9, 0x22);  // precharge periods (0xf1?)
                            // [0xd9,0x22]
}


// All writes start with a control byte.
//
// The MSB (bit 7) of the control byte is the "continuation" bit.
// It is always zero here.
//
// The next bit (bit 6) determines whether the following data bytes are
// command (bit 6 = 0) or data (bit 6 = 1).
//
// After the control bytes comes either one or two bytes of command (to be
// interpreted and processed immediately), or one or more bytes of data (to be
// written to display RAM).


void Ssd1306_128x64::write_cmd(uint8_t cmd)
{
    const uint8_t ctrl = 0x00;
    _i2c_dev.write(ctrl, cmd);
}


void Ssd1306_128x64::write_cmd(uint8_t cmd1, uint8_t cmd2)
{
    const uint8_t ctrl = 0x00;
    uint8_t buf[] = {cmd1, cmd2};
    _i2c_dev.write(ctrl, buf, sizeof(buf));
}


void Ssd1306_128x64::write_data(uint8_t *buf, int buf_len)
{
    const uint8_t ctrl = 0x40;
    _i2c_dev.write(ctrl, buf, buf_len);
}


void Ssd1306_128x64::page(int p)
{
    if (p < 0 || p >= pages)
        throw invalid_argument("page: page out of range");

    write_cmd(0xb0 | (p & 0x7));
}


void Ssd1306_128x64::column(int c)
{
    if (c < 0 || c >= cols)
        throw invalid_argument("column: column out of range");

    write_cmd(0x10 | ((c >> 4) & 0xf), c & 0xf);
}


void Ssd1306_128x64::on()
{
    write_cmd(0xaf);
}


void Ssd1306_128x64::off()
{
    write_cmd(0xae);
}


void Ssd1306_128x64::clear()
{
    memset(_image, 0, sizeof(_image));
}


void Ssd1306_128x64::flush()
{
    for (int p = 0; p < pages; p++) {
        page(p);
        column(0);
        write_data(_image[p], cols);
    }
}


// set or clear a pixel
void Ssd1306_128x64::set(int x, int y, int d)
{
    if (x < 0 || x >= cols)
        throw invalid_argument("set: x out of range");

    if (y < 0 || y >= rows)
        throw invalid_argument("set: y out of range");

    if (d != 0 && d != 1)
        throw invalid_argument("set: d out of range");

    int p = y / 8;
    assert(p < pages);
    uint8_t b = d << (y % 8);
    _image[p][x] |= b;
}


// put character at (col, row) character coordinates
// 5x7 characters are put in 6x8 cells
// col = 0 ... 20 (for 128 pixels wide)
// row = 0 ... 7 (for 64 pixels high)
void Ssd1306_128x64::putc(int col, int row, char c, uint8_t font[128][5])
{
    if (col < 0 || col >= (cols / 6))
        throw invalid_argument("putc: col out of range");

    if (row < 0 || row >= (rows / 8))
        throw invalid_argument("putc: row out of range");

    for (int i = 0; i < 5; i++)
        _image[row][col * 6 + i] = font[int(c)][i];
}


// same thing, but character is double-sized
// col, row is still the position of a single-sized character
void Ssd1306_128x64::putc2(int col, int row, char c, uint8_t font[128][5])
{
    //cout << "putc2(col=" << col << ",row=" << row << ",c=" << int(c) << ",font)" << endl;

    if (col < 0 || col >= (cols / 6 - 1))
        throw invalid_argument("putc2: col out of range");

    if (row < 0 || row >= (rows / 8 - 1))
        throw invalid_argument("putc2: row out of range");

    for (int i = 0; i < 5; i++) {
        const uint8_t lo = lo2(font[int(c)][i]);
        _image[row][col*6 + 2*i] = lo;
        _image[row][col*6 + 2*i+1] = lo;
        const uint8_t hi = hi2(font[int(c)][i]);
        _image[row+1][col*6 + 2*i] = hi;
        _image[row+1][col*6 + 2*i+1] = hi;
    }
}


void Ssd1306_128x64::puts2(int col, int row, const char *s, uint8_t font[128][5])
{
    const int s_len = strlen(s);
    for (int i = 0; i < s_len; i++)
        putc2(col+2*i, row, *s++, font);
}


// double the low bits, e.g. 0x01001001 -> 0x11000011
uint8_t Ssd1306_128x64::lo2(uint8_t b)
{
    uint8_t x = 0;
    if (b & 0x01) x |= 0x03;
    if (b & 0x02) x |= 0x0c;
    if (b & 0x04) x |= 0x30;
    if (b & 0x08) x |= 0xc0;
    return x;
}


// double the high bits, e.g. 0x01001001 -> 0x00110000
uint8_t Ssd1306_128x64::hi2(uint8_t b)
{
    uint8_t x = 0;
    if (b & 0x10) x |= 0x03;
    if (b & 0x20) x |= 0x0c;
    if (b & 0x40) x |= 0x30;
    if (b & 0x80) x |= 0xc0;
    return x;
}


// put character at (x, y) pixel coordinates
// 5x7 characters are put in 6x8 cells
// x = 0 ... 123 (for 128 pixels wide) (123 + 5 = 128)
// y = 0 ... 57 (for 64 pixels high) (57 + 7 = 64)
void Ssd1306_128x64::putc_at(int x, int y, char c, uint8_t font[128][5])
{
    if (x < 0 || x > (cols - 5))
        throw invalid_argument("putc_at: x out of range");

    if (y < 0 || y > (rows - 7))
        throw invalid_argument("putc_at: y out of range");

    // page for upper part of character (lower is p1 + 1)
    int p1 = y / 8;
    assert(p1 < pages);

    // bits from character used in upper part (lower is ~b1)
    uint8_t b1 = 0xff >> (y % 8);

    // bits in upper page where character goes (lower is ~m1)
    uint8_t m1 = 0xff << (y % 8);
    
    for (int i = 0; i < 5; i++) {
        // clear bits in image
        _image[p1][x + i] &= ~m1;
        if (p1 < (pages - 1))
            _image[p1 + 1][x + i] &= m1;
        // insert bits from char into image
        _image[p1][x + i] |= ((font[int(c)][i] & b1) << (y % 8));
        if (p1 < (pages - 1))
            _image[p1 + 1][x + i] |= ((font[int(c)][i] & ~b1) >> (8 - (y % 8)));
    }
}


// horizontal line from (x1, y) to (x2, y), including endpoints
void Ssd1306_128x64::hline(int x1, int x2, int y)
{
    if (x1 < 0 || x1 >= cols)
        throw invalid_argument("hline: x1 out of range");

    if (x2 < 0 || x2 >= cols)
        throw invalid_argument("hline: x2 out of range");

    if (y < 0 || y >= rows)
        throw invalid_argument("hline: y out of range");

    if (x1 > x2) {
        // swap
        int t = x1;
        x1 = x2;
        x2 = t;
    }

    while (x1 <= x2)
        set(x1++, y);
}


// vertical line from (x, y1) to (x, y2), including endpoints
void Ssd1306_128x64::vline(int x, int y1, int y2)
{
    if (x < 0 || x >= cols)
        throw invalid_argument("vline: x out of range");

    if (y1 < 0 || y1 >= rows)
        throw invalid_argument("vline: y1 out of range");

    if (y2 < 0 || y2 >= rows)
        throw invalid_argument("vline: y2 out of range");

    if (y1 > y2) {
        // swap
        int t = y1;
        y1 = y2;
        y2 = t;
    }

    while (y1 <= y2)
        set(x, y1++);
}


void Ssd1306_128x64::box(int x1, int y1, int x2, int y2)
{
    hline(x1, x2, y1);
    hline(x1, x2, y2);
    vline(x1, y1, y2);
    vline(x2, y1, y2);
}
