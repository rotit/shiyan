#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>

typedef struct 
{
        unsigned int  year;
        unsigned int  month;
        unsigned int  date;
        unsigned int  hour;
        unsigned int  minute;
        unsigned int  second;
        unsigned int  weekday;
} rtc_time_t;
#define HI_RTC_RD_TIME		_IOR('p', 0x09,  rtc_time_t)
#define HI_RTC_SET_TIME		_IOW('p', 0x0a,  rtc_time_t)
#define DEV "/dev/hi_rtc"


typedef struct SYSTEM_TIME{
    int  year;///< 年。
	int  month;///< 月，January = 1, February = 2, and so on.
	int  day;///< 日。
	int  wday;///< 星期，Sunday = 0, Monday = 1, and so on
	int  hour;///< 时。
	int  minute;///< 分。
	int  second;///< 秒。
	int  isdst;///< 夏令时标识。
}SYSTEM_TIME;

int SystemGetCurrentTime(SYSTEM_TIME *ptime)
{
  time_t      t = time(NULL);
  struct tm    time;
  struct timeval    tv;
  struct timezone  tz;

  gettimeofday(&tv, &tz);
  localtime_r(&t, &time);

  ptime->second  = time.tm_sec;
  ptime->minute  = time.tm_min;
  ptime->hour  = time.tm_hour;
  ptime->wday  = time.tm_wday;
  ptime->day  = time.tm_mday;
  ptime->month  = time.tm_mon + 1;
  ptime->year  = time.tm_year + 1900;

  return 0;
}

int main(int argc,char **argv)
{
	int fd=0;
	int ret=0;
	SYSTEM_TIME st;
	char option;
	SystemGetCurrentTime(&st);
	printf("system time,year=%d,month=%d,day=%d,hour=%d,min=%d,second=%d,weekday=%d\n",st.year,st.month,st.day,st.hour,st.minute,st.second,st.wday);

	fd=open(DEV,O_RDWR);
	if(fd<0)
	{
		printf("open error\n");
	}
	rtc_time_t time;		
	
	ret=ioctl(fd,HI_RTC_RD_TIME,&time);
	if(ret<0)
		printf("ioctl error\n");
	printf("rtc time,year=%d,month=%d,date=%d,hour=%d,minute=%d,second=%d,weekday=%d\n",time.year,time.month,time.date,time.hour,time.minute,time.second,time.weekday);


	rtc_time_t settime;		
	memset(&settime,0,sizeof(settime));
	settime.year=st.year;
	settime.month=st.month;
	settime.date=st.day;
	settime.weekday=st.wday;
	settime.hour=st.hour;
	settime.minute=st.minute;
	settime.second=st.second;
	ret=ioctl(fd,HI_RTC_SET_TIME,&settime);
	if(ret<0)
		printf("ioctl error\n");
	option=*argv[1];

	switch (option)
	{
		case '0':
			break;
		case '1':
			ret=ioctl(fd,HI_RTC_RD_TIME,&time);
			if(ret<0)
				printf("ioctl error\n");
			printf("after set time,year=%d,month=%d,date=%d,hour=%d,minute=%d,second=%d,weekday=%d\n",time.year,time.month,time.date,time.hour,time.minute,time.second,time.weekday);
			break;
		default:
			printf("error param\0");
	}
	close(fd);
}

