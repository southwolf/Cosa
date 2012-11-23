/**
 * @file Cosa/ADXL345.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Analog Devices ADXL345 Digital Accelerometer SPI driver.
 * http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/ADXL345.h"

ADXL345::ADXL345(uint8_t ss) : 
  SPI(), 
  _ss(ss, 1) 
{
  begin(SPI::DIV4_CLOCK, 3, SPI::MSB_FIRST);
  write(DATA_FORMAT, _BV(FULL_RES) | RANGE_16G);
  write(POWER_CTL, _BV(MEASURE));
  calibrate(0, 0, 0);
}

void 
ADXL345::write(Register reg, uint8_t value)
{
  SPI_transaction(_ss) {
    SPI::write(WRITE_CMD | (reg & REG_MASK), value);
  }
}

void 
ADXL345::write(Register reg, void* buffer, uint8_t count)
{
  SPI_transaction(_ss) {
    SPI::write(WRITE_CMD | MULTIPLE_BYTE | (reg & REG_MASK), buffer, count);
  }
}

uint8_t 
ADXL345::read(Register reg)
{
  uint8_t res;
  SPI_transaction(_ss) {
    res = SPI::read(READ_CMD | (reg & REG_MASK));
  }
  return (res);
}

void 
ADXL345::read(Register reg, void* buffer, uint8_t count)
{
  SPI_transaction(_ss) {
    SPI::read(READ_CMD | MULTIPLE_BYTE | (reg & REG_MASK), buffer, count);
  }
}

void 
ADXL345::calibrate(int8_t x, int8_t y, int8_t z)
{
  offset_t ofs; 
  ofs.x = x;
  ofs.y = y;
  ofs.z = z;
  write(OFS, &ofs, sizeof(ofs));
}

void 
ADXL345::sample(sample_t& s)
{
  read(DATA, &s, sizeof(s));
}

void
ADXL345::calibrate()
{
  sample_t smpl;
  calibrate(0, 0, 0);
  sample(smpl);
  calibrate(-smpl.x/4, -smpl.y/4, -smpl.z/4);
}

void 
ADXL345::sample_t::print(IOStream& stream)
{
  stream.printf_P(PSTR("ADXL345::sample_t(x = %d, y = %d, z = %d)"), x, y, z);
}

void 
ADXL345::sample_t::println(IOStream& stream)
{
  print(stream);
  stream.println();
}

