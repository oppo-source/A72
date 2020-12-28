/***************************************************
 * File:touch_interfaces.h
 * VENDOR_EDIT
 * Copyright (c)  2008- 2030  CUSTOM_O Mobile communication Corp.ltd.
 * Description:
 *             Touch interface
 * Version:1.0:
 * Date created:2016/09/02
 * Author: Tong.han@Bsp.Driver
 * TAG: BSP.TP.Init
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * INFORMATION CONTAINED IN THIS DOCUMENT IS PROVIDED "AS-IS," AND SYNAPTICS
 * EXPRESSLY DISCLAIMS ALL EXPRESS AND IMPLIED WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * AND ANY WARRANTIES OF NON-INFRINGEMENT OF ANY INTELLECTUAL PROPERTY RIGHTS.
 * IN NO EVENT SHALL SYNAPTICS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, PUNITIVE, OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OF THE INFORMATION CONTAINED IN THIS DOCUMENT, HOWEVER CAUSED
 * AND BASED ON ANY THEORY OF LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, AND EVEN IF SYNAPTICS WAS ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE. IF A TRIBUNAL OF COMPETENT JURISDICTION DOES
 * NOT PERMIT THE DISCLAIMER OF DIRECT DAMAGES OR ANY OTHER DAMAGES, SYNAPTICS'
 * TOTAL CUMULATIVE LIABILITY TO ANY PARTY SHALL NOT EXCEED ONE HUNDRED U.S.
 * DOLLARS.
 * *
 * -------------- Revision History: -----------------
 *  <author >  <data>  <version>  <desc>
 ***************************************************/

#ifndef TOUCH_INTERFACES_H
#define TOUCH_INTERFACES_H
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#define MAX_I2C_RETRY_TIME 2

//---SPI READ/WRITE---
#define SPI_WRITE_MASK(a) (a | 0x80)
#define SPI_READ_MASK(a) (a & 0x7F)
#define DUMMY_BYTES (1)
#define SPI_TANSFER_LEN (63 * 1024)
#define SPI_READ_LEN (1032)

enum SPI_RW {
	SPIWRITE = 1,
	SPIREAD = 2
};

int touch_i2c_read_byte(struct i2c_client *client, u16 addr);
int touch_i2c_write_byte(struct i2c_client *client, u16 addr,
			 unsigned char data);

int touch_i2c_read_word(struct i2c_client *client, u16 addr);
int touch_i2c_write_word(struct i2c_client *client, u16 addr,
			 unsigned short data);

int touch_i2c_read_block(struct i2c_client *client, u16 addr,
			 unsigned short length, unsigned char *data);
int touch_i2c_write_block(struct i2c_client *client, u16 addr,
			  unsigned short length, unsigned char const *data);

int touch_i2c_read(struct i2c_client *client, char *writebuf, int writelen,
		   char *readbuf, int readlen);
int touch_i2c_write(struct i2c_client *client, char *writebuf, int writelen);

int init_touch_interfaces(struct device *dev, bool flag_register_16bit);
int touch_i2c_continue_read(struct i2c_client *client, unsigned short length,
			    unsigned char *data);
int touch_i2c_continue_write(struct i2c_client *client, unsigned short length,
			     unsigned char *data);
int32_t spi_read_write(struct spi_device *client, uint8_t *buf, size_t len,
		       uint8_t *rbuf, enum SPI_RW rw);
int32_t CTP_SPI_READ(struct spi_device *client, uint8_t *buf, uint16_t len);
int32_t CTP_SPI_WRITE(struct spi_device *client, uint8_t *buf, uint16_t len);

int spi_write_firmware(struct spi_device *client, u8 *fw, u32 *len_array,
		       u8 array_len);

#endif
