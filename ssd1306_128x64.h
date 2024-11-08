#pragma once

#include <cstdint>

class I2cDev;


class Ssd1306_128x64
{
  public:

    Ssd1306_128x64(I2cDev& i2c_dev);

    static const int rows = 64;
    static const int cols = 128;

    void on();
    void off();
    void clear();
    void flush();
    void set(int x, int y, int d=1);
    void putc(int x, int y, char c, uint8_t font[128][5]);
    void puts(int x, int y, const char *s, uint8_t font[128][5]);
    void putc2(int col, int row, char c, uint8_t font[128][5]);
    void puts2(int col, int row, const char *s, uint8_t font[128][5]);
    void putc_at(int x, int y, char c, uint8_t font[128][5]);
    void hline(int x1, int x2, int y);
    void vline(int x, int y1, int y2);
    void box(int x1, int y1, int x2, int y2);
    void fill(int x1, int y1, int x2, int y2);

  private:

    static const bool flip_h = true;
    static const bool flip_v = true;

    I2cDev& _i2c_dev;

    static const int pages = rows / 8;

    uint8_t _image[pages][cols];

    void write_cmd(uint8_t cmd);
    void write_cmd(uint8_t cmd1, uint8_t cmd2);
    void write_data(uint8_t *buf, int buf_len);

    void page(int p);
    void column(int c);

    static uint8_t lo2(uint8_t b);
    static uint8_t hi2(uint8_t b);
};
