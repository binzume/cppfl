
#include <string>
#include <time.h>

class Date
{
public:
	int year;
	int month;
	int day;
	Date(){year=month=day=0;}
	Date(int y,int m,int d) {year=y;month=m;day=d;}
	static Date today(){
		time_t ti;
		time(&ti);
		struct tm *t_st = localtime(&ti);
		return Date(t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday);
	}


//	static int mdays[]={31,28,31,30,31,30};
	bool is_leap()
	{
		return (year%4==0 && year%100!=0) || year%400==0;
	}

	operator std::string() {
		char s[20];
		sprintf(s,"%d-%02d-%02d",year,month,day);
		return s;
	}

};

// Time‚ÆSecond‚ğì‚ê

class Second
{
public:
	int sec;
	Second(int second=0){sec=second;}
	static Second now(){
		time_t ti;
		time(&ti);
		struct tm *t_st = localtime(&ti);
		return Second(t_st->tm_hour*3600+t_st->tm_min*60+t_st->tm_sec);
	}
	int hour(){return sec/3600;}
	int minute(){return sec/60%60;}
	int second(){return sec%60;}
	
	operator int(){return sec;}
	Second operator++(int){
		sec++;
		return *this;
	}

};

class Time
{
public:
	int hour;
	int minute;
	int second;

	Time(){hour=minute=second=0;}
	Time(int s){hour=s/3600;minute=s/60%60;second=s%60;}
	Time(int h,int m,int s=0){
		if (s<0) {m--;s+=60;}
		if (m<0) {h--;m+=60;}
		m+=s/60; s%=60;
		h+=m/60; m%=60;
		hour=h;minute=m;second=s;
	}
	static Time now(){
		time_t ti;
		time(&ti);
		struct tm *t_st = localtime(&ti);
		return Time(t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
	}

	Time& operator+=(const Time &t2) {
		return *this;
	}
	Time& operator-=(const Time &t) {
		return *this;
	}

	Time operator+(const Time &t) {
		return Time();
	}

	Time operator-(const Time &t) {
		return Time(hour-t.hour,minute-t.minute,second-t.second);
	}

	operator std::string() {
		char s[20];
		sprintf(s,"%02d:%02d:%02d",hour,minute,second);
		return s;
	}

};

class DateTime
{
public:
	Date date;
	Time time;
	DateTime(const Date &d, const Time &t) {date=d;time=t;}
	static DateTime now(){
		::time_t ti;
		::time(&ti);
		struct tm *t_st = localtime(&ti);
		return DateTime(Date(t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday), Time(t_st->tm_hour,t_st->tm_min,t_st->tm_sec));
	}
	operator std::string() {
		return (std::string)date+" "+(std::string)time;
	}
};

