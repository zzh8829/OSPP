#include "time.h"
#include "cpu.h"

#define BCD2BIN(bcd) ((((bcd)&15) + ((bcd)>>4)*10))
#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

void read_rtc_time(struct tm * timeptr)
{
	cli();
	do {
		timeptr->tm_sec = BCD2BIN(read_CMOS(0x0));
		timeptr->tm_min = BCD2BIN(read_CMOS(0x2));
		timeptr->tm_hour = BCD2BIN(read_CMOS(0x4));
		timeptr->tm_mday = BCD2BIN(read_CMOS(0x7));
		timeptr->tm_mon = BCD2BIN(read_CMOS(0x8));
		timeptr->tm_year = BCD2BIN(read_CMOS(0x9));
	} while(0);// timeptr->tm_sec != BCD2BIN(read_CMOS(0x0)));
	sti();
}

time_t mktime (struct tm * timeptr)
{
	long res;
	int year;
	
	int month[12] = {
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
	};

	year = timeptr->tm_year - 70;
/* magic offsets (y+1) needed to get leapyears right.*/
	res = YEAR*year + DAY*((year+1)/4);
	res += month[timeptr->tm_mon];
/* and (y+2) here. If it wasn't a leap-year, we have to adjust */
	if (timeptr->tm_mon>1 && ((year+2)%4))
		res -= DAY;
	res += DAY*(timeptr->tm_mday-1);
	res += HOUR*timeptr->tm_hour;
	res += MINUTE*timeptr->tm_min;
	res += timeptr->tm_sec;
	return res;
}

time_t time (time_t* timer)
{
	time_t t;
	struct tm timeinfo;
	read_rtc_time(&timeinfo);
	t = mktime(&timeinfo);
	if(timer)*timer = t;
	return t;
}