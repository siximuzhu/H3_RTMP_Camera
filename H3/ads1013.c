#include "ads1013.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <time.h>
#include <sys/time.h>

#define L2B16(Litte)  (((Litte & 0xff) << 8) | ((Litte >> 8) & 0xff))

int battery_arry[100] = {
	1500,1525,1539,1547,1554,1559,1563,1568,1572,1577,
	1582,1587,1591,1595,1598,1602,1605,1607,1611,1613,
	1616,1618,1620,1622,1624,1626,1628,1630,1632,1633,
	1634,1635,1638,1639,1641,1643,1645,1646,1648,1650,
	1652,1654,1656,1658,1660,1662,1665,1667,1670,1673,
	1676,1679,1683,1685,1690,1694,1699,1703,1708,1713,
	1717,1723,1728,1733,1737,1742,1746,1751,1755,1759,
	1763,1767,1771,1774,1779,1783,1787,1791,1795,1799,
	1802,1806,1811,1815,1819,1823,1828,1831,1835,1839,
	1844,1848,1852,1857,1862,1867,1870,1878,1885,1891
};

int voltage_to_electri_quantity(float voltage)
{
	int v,i;

	v = voltage * 1000;
	for(i = 0;i < 100;i++)
	{
		if(v > battery_arry[i])
			continue;
		else
			break;
	}
	return i;
}




void sysLocalTime() 
{ 
	time_t             timesec; 
	struct tm         *p; 
	
	time(timesec); 
	p = localtime(timesec); 


	printf("%d-%d-%d-%d-%d-%d:", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec); 


} 



int ads1013_init(void)
{
	int fd;

	fd = wiringPiI2CSetup(ADS1013_ADDRESS);
	if(fd < 0)
	{
		printf("I2C setup error!\n");
		return fd;
	}
	else
	{
		printf("I2C setup ok!\n");
	}

	
	return fd;
}


short ads1013_read(int fd)
{
	short reg_config = 0;
	short value = 0;
	int reg;
	
	reg_config = wiringPiI2CReadReg16(fd,ADS1013_REG_POINTER_CONFIG);

	reg_config |= ADS1013_REG_CONFIG_OS_SINGLE;

	reg = wiringPiI2CWriteReg16(fd,ADS1013_REG_POINTER_CONFIG,reg_config);

	delay(5);//5ms	
	value = wiringPiI2CReadReg16(fd,ADS1013_REG_POINTER_CONVERT);

	value = L2B16(value);
//	printf("adc row value = 0x%04x\n",value);

	return (value >> 4);
}

float get_baterry_voltage(int fd)
{
	int i;
	int total = 0;
	float voltage;

	for(i = 0;i < 5;i++)
	{
		total += ads1013_read(fd);	
	}	
	voltage = 2.048 * (total / 5.0 / 2048.0);

	return voltage;
}

int get_power(int fd)
{
	short tmp;
	float voltage;
	int i;
	int power;

//	fd = ads1013_init();

//	for(i = 0; ;i++)
	{
//		tmp = ads1013_read(fd);
//		sysLocalTime();
//		voltage = 2.048 * (tmp / 2048.0);
		voltage = get_baterry_voltage(fd);

//		printf("[%d]:vlotage = %fV  power:%d\%\n",i,voltage,voltage_to_electri_quantity(voltage));	
		power = voltage_to_electri_quantity(voltage);
//		delay(1000);
	}
	return power;
}




