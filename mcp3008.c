/* Copyright 2014 Drew Thoreson
 *
 * Some code stolen from wiringPi, Copyright (c) 2012 Gordon Henderson
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "mcp3008.h"

static const unsigned char spi_mode  = 0;
static const unsigned char spi_bpw   = 8;
static const unsigned int  spi_delay = 0;
static const unsigned int  spi_speed = 1000000;

int mcp3008_open(const char *dev)
{
	int fd;

	if ((fd = open(dev, O_RDWR)) < 0)
		return -1;
	if (ioctl(fd, SPI_IOC_WR_MODE, &spi_mode) < 0)
		return -1;
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0)
		return -1;
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0)
		return -1;
	return fd;
}

static int spi_rw(int spi_fd, unsigned char *data, int len)
{
	struct spi_ioc_transfer spi;

	spi.tx_buf        = (unsigned long) data;
	spi.rx_buf        = (unsigned long) data;
	spi.len           = len;
	spi.delay_usecs   = spi_delay;
	spi.speed_hz      = spi_speed;
	spi.bits_per_word = spi_bpw;

	return ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
}

/* read a 10-bit integer from the MCP3008 ADC */
int mcp3008_read(int spi_fd, int adc_channel)
{
	unsigned char data[3];
	unsigned int val = 0;

	data[0] = 1;
	data[1] = 0x80 | ((adc_channel & 7) << 4);
	data[2] = 0;

	if (spi_rw(spi_fd, data, 3) < 0)
		return -1;

	val = (data[1] << 8) & 0x300;
	val |= data[2] & 0xFF;
	return val;
}
