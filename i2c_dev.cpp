
#include <cstdint>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "i2c_dev.h"


I2cDev::I2cDev(const char *i2c_dev, uint8_t i2c_adr, int max_msg) :
    _i2c_fd(-1),
    _i2c_adr(i2c_adr),
    _buf_max(0),
    _buf(nullptr)
{
    if (max_msg <= 0)
        return;

    _i2c_fd = open(i2c_dev, O_RDWR);
    if (_i2c_fd < 0)
        return;

    // one byte for the reg adr, then the data
    _buf_max = max_msg + 1;
    _buf = new uint8_t[_buf_max];
}


I2cDev::~I2cDev()
{
    if (_i2c_fd >= 0) {
        close(_i2c_fd);
        _i2c_fd = -1;
    }

    delete[] _buf;
    _buf = nullptr;
    _buf_max = 0;
}


// Read buf_size bytes from reg_adr into buf
//
// If the read fails, retry, up to max_tries total tries.
// max_tries=0 (default) means try forever.
//
// Returns:
//   on success, the number of times we had to read (1 means first try)
//   -1 if we get to max_tries without succeeding (or i2c device error)
int I2cDev::read(uint8_t reg_adr, uint8_t *buf, int buf_size, int max_tries)
{
    if (_i2c_fd < 0)
        return -1;

    // one to send the reg adrs, one to receive the data
    i2c_msg msgs[2] = {
        { _i2c_adr, 0, 1, &reg_adr },
        { _i2c_adr, I2C_M_RD, uint16_t(buf_size), buf }
    };
    i2c_rdwr_ioctl_data rdwr = { msgs, 2 };

    if (buf == nullptr || buf_size == 0)
        rdwr.nmsgs = 1; // just sending the register address, ok

    int attempts = 0;
    while (max_tries == 0 || attempts < max_tries) {
        attempts++;
        if (ioctl(_i2c_fd, I2C_RDWR, &rdwr) >= 0)
            return attempts;
    }

    return -1;
}


int I2cDev::read(uint8_t reg_adr, uint8_t& reg_val, int max_tries)
{
    return read(reg_adr, &reg_val, sizeof(reg_val), max_tries);
}


int I2cDev::write(uint8_t reg_adr, uint8_t *buf, int buf_size, int max_tries)
{
    if (_buf == nullptr || _buf_max <= 0)
        return -1;

    if (_i2c_fd < 0)
        return -1;

    if (buf_size >= _buf_max)
        return -1;

    _buf[0] = reg_adr;
    if (buf_size > 0)
        memcpy(&_buf[1], buf, buf_size);

    i2c_msg msgs[1] = {
        { _i2c_adr, 0, uint16_t(buf_size + 1), _buf }
    };
    i2c_rdwr_ioctl_data rdwr = { msgs, 1 };

    int attempts = 0;
    while (max_tries == 0 || attempts < max_tries) {
        attempts++;
        if (ioctl(_i2c_fd, I2C_RDWR, &rdwr) >= 0)
            return attempts;
    }

    return -1;
}


int I2cDev::write(uint8_t reg_adrs, uint8_t reg_val, int max_tries)
{
    return write(reg_adrs, &reg_val, sizeof(reg_val), max_tries);
}
