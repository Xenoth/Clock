#include "DS3231.h"

DS3231::DS3231()
{
	detected=true;
}



uint8_t DS3231::decToBcd(uint8_t val)
{
	return (uint8_t)((val/10*16)+(val%10));
}

uint8_t DS3231::bcdToDec(uint8_t val)
{
	return (uint8_t)((val/16*10)+(val%16));
}




uint8_t DS3231::getSecond()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x00));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS, 1);
	if(Wire.available()==0)
	{
		detected=false;
		return 255;
	}
	detected=true;
	return bcdToDec(Wire.read());
}

uint8_t DS3231::getMinute()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x01));
	if(Wire.endTransmission())
  {
    detected=false;
    return 255;
  }
	Wire.requestFrom(I2C_ADRESS, 1);
	detected=true;
	return bcdToDec(Wire.read());
}

uint8_t DS3231::getHour(bool *mode12h, bool *pm)
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x02));
	if(Wire.endTransmission())
  {
    detected=false;
    return 255;
  }
	Wire.requestFrom(I2C_ADRESS, 1);
	detected=true;
	uint8_t result=Wire.read();

	//If 12-hour mode
	if(result & 0b01000000)
	{
		*mode12h=true;
		if(result & 0b00100000)
		{
			*pm=true;
		}
		else
			*pm=false;
		return bcdToDec(result & 0b00011111);
	}
	else
	{
		*mode12h=false;
		return bcdToDec(result & 0b00111111);
	}
}

int DS3231::getCurrentTime(bool *mode12h, bool *pm, uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x00));
	if(Wire.endTransmission())
  {
    detected=false;
    return 1;
  }
	Wire.requestFrom(I2C_ADRESS, 3);
	detected=true;

	*second=bcdToDec(Wire.read());
	*minute=bcdToDec(Wire.read());
	uint8_t resultHour=Wire.read();

	//If 12-hour mode
	if(resultHour & 0b01000000)
	{
		*mode12h=true;
		if(resultHour & 0b00100000)
		{
			*pm=true;
		}
		else
			*pm=false;
		*hour=bcdToDec(resultHour & 0b00011111);
	}
	else
	{
		*mode12h=false;
		*hour=bcdToDec(resultHour & 0b00111111);
	}
}

bool DS3231::setSecond(const uint8_t second)
{
	if(second<0 || second>59)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x00));
	Wire.write(decToBcd(second));
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }
	return true;
}

bool DS3231::setMinute(const uint8_t minute)
{
	if(minute<0 || minute>59)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x01));
	Wire.write(decToBcd(minute));
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }
	return true;
}

bool DS3231::setHour(const uint8_t hour, const bool mode12h, const bool pm)
{
  uint8_t byteHour;
	if(mode12h)
	{
		if(hour<0 || hour>12)
			return false;
		byteHour=decToBcd(hour);
		byteHour=(byteHour | 0b0100000);

		if(pm)
		{
			byteHour=(byteHour | 0b0010000);
		}
	}
	else
	{
		if(hour<0 || hour>23)
			return false;
		  byteHour=decToBcd(hour);
	}

	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x02));
	Wire.write(byteHour);
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }
	return true;
}

uint8_t DS3231::getDayOfWeek()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x03));
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }

	Wire.requestFrom(I2C_ADRESS,1);
	detected=true;
	return bcdToDec(Wire.read());
}

uint8_t DS3231::getDayOfMonth()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x04));
	if(Wire.endTransmission())
  {
    detected=false;
    return 255;
  }

	Wire.requestFrom(I2C_ADRESS,1);
	
	detected=true;
	return bcdToDec(Wire.read());
}

uint8_t DS3231::getMonth()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	if(Wire.endTransmission())
  {
    detected=false;
    return 255;
  }

	Wire.requestFrom(I2C_ADRESS,1);
	
	detected=true;
	return bcdToDec((Wire.read())&0b0111111);
}

uint8_t DS3231::getYear()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	if(Wire.endTransmission())
  {
    detected=false;
    return 255;
  }

	Wire.requestFrom(I2C_ADRESS,2);
	
	detected=true;
	bool century=false;
	if((Wire.read())&0b1000000)
		century=true;
	uint8_t year=bcdToDec(Wire.read());
	if (century)
		year=year+100;
	return year;
}

int DS3231::getDate(uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x03));
	if(Wire.endTransmission())
  {
    detected=false;
    return 1;
  }

	Wire.requestFrom(I2C_ADRESS, 4);
	detected=true;
	*dayOfWeek=bcdToDec(Wire.read());
	*dayOfMonth=bcdToDec(Wire.read());
	uint8_t byteMonth=Wire.read();
	bool century=false;
	if(byteMonth&0b10000000)
		century=true;
	*month=bcdToDec(byteMonth&0b01111111);
	*year=bcdToDec(Wire.read());
	if (century)
		*year=*year+100;
  return 0;
}

bool DS3231::setDayOfMonth(const uint8_t dayOfMonth)
{
	if(dayOfMonth<1 || dayOfMonth>31)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x04));
	Wire.write(decToBcd(dayOfMonth));
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }
	return true;
}

bool DS3231::setMonth(const uint8_t month)
{
	if(month<1 || month>12)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }

	Wire.requestFrom(I2C_ADRESS,1);
	detected=true;
	uint8_t oldByte=(Wire.read()&0b10000000);

	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	Wire.write(decToBcd(month) | oldByte);
	if(Wire.endTransmission())
  {
    detected=false;
    return false;
  }
	return true;
}

bool DS3231::setYear(const uint8_t year)
{
	if(year<0 || year>199)
		return false;
    uint8_t yearCopy=year;
	if(year>99)
	{
		Wire.beginTransmission(I2C_ADRESS);
		Wire.write(uint8_t(0x05));
		if(Wire.endTransmission())
    {
      detected=false;
       return false;
    }

		Wire.requestFrom(I2C_ADRESS,1);
		detected=true;
		uint8_t oldByte=(Wire.read()&0b01111111);
		yearCopy=yearCopy-100;
		Wire.beginTransmission(I2C_ADRESS);
		Wire.write(uint8_t(0x05));
		Wire.write(0b10000000 | oldByte);
		if(Wire.endTransmission())
    {
      detected=false;
       return false;
    }
	}
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x06));
	Wire.write(decToBcd(yearCopy));
	if(Wire.endTransmission())
  {
      detected=false;
      return false;
  }
	return true;
}
