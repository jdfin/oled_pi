#pragma once

#include <cstdint>


// Set frequency in /boot/config.txt:
// dtparam=i2c_arm=on,i2c_arm_baudrate=400000
//
// Prevent cpu frequency scaling from slowing it down:
// $ echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
//
// Setting it for cpu0 sets it for all CPUs.
//
// Undo "performance":
// $ echo ondemand | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
//
// If you don't set "performance", i2c typically runs at about half
// speed, which might be fine in many cases.
//
// Transactions appear to block in the ioctl, so the duration of a transaction is
// roughly 9 * (number_of_bytes) / clock_freq.


class I2cDev {

    public:

        I2cDev(const char *i2c_dev, uint8_t i2c_adr, int max_msg=32);

        virtual ~I2cDev();

    //protected:

        int read(uint8_t reg_adrs, uint8_t *buf, int buf_size=1, int max_tries=1);

        int read(uint8_t reg_adrs, uint8_t& reg_val, int max_tries=1);

        int write(uint8_t reg_adrs, uint8_t *buf, int buf_size=1, int max_tries=1);

        int write(uint8_t reg_adrs, uint8_t reg_val, int max_tries=1);

    private:

        int _i2c_fd;

        uint8_t _i2c_adr;

        // _buf is used to combine register address (_buf[0]) and user
        // data (_buf[1...]) so we can write with one i2c transaction
        int _buf_max;
        uint8_t *_buf;
};
