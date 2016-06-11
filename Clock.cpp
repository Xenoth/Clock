#include "Clock.h"

Clock::Clock()
{
	detected=true;
}



uint8_t Clock::decToBcd(uint8_t val)
{
	return (uint8_t) ((val / 10 * 16) + (val % 10));
}

uint8_t Clock::bcdToDec(uint8_t val)
{
	return (uint8_t) ((val / 16 * 10) + (val % 16));
}



inline bool Clock::isDetected()
{
	return detected;
}



uint8_t Clock::getSecond()
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

uint8_t Clock::getMinute()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x01));
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

uint8_t Clock::getHour(bool *mode12h, bool *pm)
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x02));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS, 1);
	if(Wire.available()==0)
	{
		detected=false;
		return 255;
	}
	detected=true;
	uint8_t result=Wire.read();

	//If 12-hour mode
	if(result & 0b0100000)
	{
		*mode12h=true;
		if(result & 0b0010000)
		{
			*pm=true;
		}
		else
			*pm=false;
		return bcdToDec(result & 0b0001111);
	}
	else
	{
		*mode12h=false;
		return bcdToDec(result & 0b0011111);
	}
}

void Clock::getCurrentTime(bool *mode12h, bool *pm, uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x00));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS, 3);
	if(Wire.available()<3)
	{
		detected=false;
		exit(1);
	}
	detected=true;

	*second=bcdToDec(Wire.read());
	*minute=bcdToDec(Wire.read());
	uint8_t resultHour=Wire.read();

	//If 12-hour mode
	if(resultHour & 0b0100000)
	{
		*mode12h=true;
		if(resultHour & 0b0010000)
		{
			*pm=true;
		}
		else
			*pm=false;
		*hour=bcdToDec(resultHour & 0b0001111);
	}
	else
	{
		*mode12h=false;
		*hour=bcdToDec(resultHour & 0b0011111);
	}
}

bool Clock::setSecond(const uint8_t second)
{
	if(second<0 || second>59)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x00));
	Wire.write(decToBcd(second));
	Wire.endTransmission();
	return true;
}

bool Clock::setMinute(const uint8_t minute)
{
	if(minute<0 || minute>59)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x01));
	Wire.write(decToBcd(minute));
	Wire.endTransmission();
	return true;
}

bool Clock::setHour(const uint8_t hour, const bool mode12h, const bool pm)
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
	Wire.endTransmission();
	return true;
}

uint8_t Clock::getDayOfWeek()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x03));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS,1);
	if(Wire.available()<1)
	{
		detected=false;
		return 255;
	}
	detected=true;
	return bcdToDec(Wire.read());
}

uint8_t Clock::getDayOfMonth()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x04));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS,1);
	if(Wire.available()<1)
	{
		detected=false;
		return 255;
	}
	detected=true;
	return bcdToDec(Wire.read());
}

uint8_t Clock::getMonth()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS,1);
	if(Wire.available()<1)
	{
		detected=false;
		return 255;
	}
	detected=true;
	return bcdToDec((Wire.read())&0b0111111);
}

uint8_t Clock::getYear()
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS,2);
	if(Wire.available()<2)
	{
		detected=false;
		return 255;
	}
	detected=true;
	bool century=false;
	if((Wire.read())&0b1000000)
		century=true;
	uint8_t year=bcdToDec(Wire.read());
	if (century)
		year=year+100;
	return year;
}

void Clock::getDate(uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x03));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS, 4);
	if(Wire.available()<4)
	{
		detected=false;
		exit(1);
	}
	detected=true;
	*dayOfWeek=bcdToDec(Wire.read());
	*dayOfMonth=bcdToDec(Wire.read());
	uint8_t byteMonth=Wire.read();
	bool century=false;
	if(byteMonth&0b1000000)
		century=true;
	*month=bcdToDec(byteMonth&0b0111111);
	*year=bcdToDec(Wire.read());
	if (century)
		*year=*year+100;
}

bool Clock::setDayOfMonth(const uint8_t dayOfMonth)
{
	if(dayOfMonth<1 || dayOfMonth>31)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x04));
	Wire.write(decToBcd(dayOfMonth));
	Wire.endTransmission();
	return true;
}

bool Clock::setMonth(const uint8_t month)
{
	if(month<1 || month>12)
		return false;
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	Wire.endTransmission();

	Wire.requestFrom(I2C_ADRESS,1);
	if(Wire.available()<1)
	{
		detected=false;
		return false;
	}
	detected=true;
	uint8_t oldByte=(Wire.read()&0b1000000);

	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x05));
	Wire.write(decToBcd(month) | oldByte);
	Wire.endTransmission();
	return true;
}

bool Clock::setYear(const uint8_t year)
{
	if(year<0 || year>199)
		return false;
    uint8_t yearCopy=year;
	if(year>99)
	{
		Wire.beginTransmission(I2C_ADRESS);
		Wire.write(uint8_t(0x05));
		Wire.endTransmission();

		Wire.requestFrom(I2C_ADRESS,1);
		if(Wire.available()<1)
		{
			detected=false;
			return false;
		}
		detected=true;
		uint8_t oldByte=(Wire.read()&0b0111111);
		yearCopy=yearCopy-100;
		Wire.beginTransmission(I2C_ADRESS);
		Wire.write(uint8_t(0x05));
		Wire.write(0b1000000 | oldByte);
		Wire.endTransmission();
	}
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(uint8_t(0x06));
	Wire.write(decToBcd(yearCopy));
	Wire.endTransmission();
	return true;
}
