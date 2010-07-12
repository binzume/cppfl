#ifndef _ENCODE_H
#define _ENCODE_H

#include <string>


namespace Encode{

#ifdef _WIN32
#include <windows.h>

typedef int ENCODE;
static const ENCODE AUTO=0; // NOT SUPPORT
static const ENCODE SJIS=932;
static const ENCODE UTF8=CP_UTF8;
static const ENCODE EUCJP=51932; // NOT SUPPORT

static std::string encode(const std::string &src,const ENCODE &from, const ENCODE &to)
{
	int len = MultiByteToWideChar(from,0,src.c_str(),src.length(),NULL,0);
	
	wchar_t *buf1 = new wchar_t[len];
	len = MultiByteToWideChar(from,0,src.c_str(),src.length(),buf1,len);

	int len2=WideCharToMultiByte(to,0,buf1,len,NULL,0,NULL,NULL);
	char *buf2 = new char[len2+1];

	len=WideCharToMultiByte(to,0,buf1,len,buf2,len2,NULL,NULL);
	buf2[len2]=0;

	std::string s=buf2;
	delete [] buf1;
	delete [] buf2;
	return s;
}

// iconv
#else
#include <iconv.h>

typedef const char* ENCODE;
static const ENCODE AUTO=NULL;
static const ENCODE SJIS="SHIFT_JIS";
static const ENCODE UTF8="UTF-8";
static const ENCODE EUCJP="EUC-jp";

static std::string encode(const std::string &src,const ENCODE &to, const ENCODE &from)
{
	std::string s;
	const char *lps = src.c_str();
	size_t len = src.length();
	char dst[256];
	char *dst2 = dst;
	size_t size = (size_t)sizeof(dst);

	iconv_t c = iconv_open(from,to);

	do {
		if (iconv(c,&lps,&len ,&dst2 ,&size) <0 ) return s; // error
		s.append(dst,sizeof(dst)-size);
		dst2=dst;
		size = (size_t)sizeof(dst);
	} while(len);

	iconv_close(c);

	return s;
}
#endif


class IStringIterator{
public:
	virtual int shift() = 0;
	virtual bool eof() = 0;
};

template<typename T>
class StringIteratorProxy : public IStringIterator{
	T sf;
public:
	StringIteratorProxy(const T &s):sf(s){}
	StringIteratorProxy(typename const T::iterator &begin, typename const T::iterator &end):sf(begin,end){}
	int shift(){return sf.shift();}
	bool eof(){return sf.eof();}
	int operator *(){
		typename T::iterator tmp = sf.it;
		return sf.shift(tmp);
	}
	bool operator !=(const typename T::iterator &s) {
		return sf.it!=s;
	}
	StringIteratorProxy& operator ++() {
		sf.shift();
		return *this;
	}
	StringIteratorProxy operator ++(int) {
		StringIteratorProxy tmp=*this;
		sf.shift();
		return tmp;
	}

};

template<typename T>
class EmptyStringIterator{
	T it,end;
public:
	EmptyStringIterator(T begin_,T end_):it(begin_),end(end_) {};
	int shift(T &it){int a=*it;++it; return a;}
	bool eof(){return it==end;}
	int shift(){
		return shift(it);
	}
};



template<typename T>
class StringIterator_SJIS{
	friend StringIteratorProxy< StringIterator_SJIS >;
	T it,end;
	inline bool isFirstByte(unsigned char c) {
		return ((unsigned char)((c^0x20)-0xA1)<=0x3B);
	}
public:
	typedef T iterator;
	StringIterator_SJIS(T begin_,T end_):it(begin_),end(end_) {};
	int shift(T &it){
		if (it==end) return -1;
		int c = (unsigned char)*it;
		++it;
		if (it==end) return c;
		if( isFirstByte(c) ) {
			c = (unsigned char)(*it) | c<<8;
			++it;
		}
		//char j1 = (s0<<1) - (s0 <= 0x9f ? 0xe0:0x160) - (s1 < 0x9f ? 1:0);
		//char j2 = s1 - 0x1f - (s1 >= 0x7f ? 1:0) - (s1 >= 0x9f ? 0x5e:0);
		//c = ((j1&0x7f)<<7) | (j2&0x7f);
		return c;
	}
	int shift(){
		return shift(it);
	}
	bool eof(){return it==end;}
};

template<typename T>
class StringIterator_UTF8{
	friend StringIteratorProxy< StringIterator_UTF8<T> >;
	T it,end;
public:
	typedef T iterator;
	StringIterator_UTF8(const T &begin_,const T &end_):it(begin_),end(end_) {};
	int shift(T &it){
		if (it==end) return -1;
		int code,bits,i=0;
		code = *it; ++it;
		int c=code;
		i++;
		bits=6;
		if (c&0x80) {
			while(c&0x40) {
				code=(code<<6)|(*it&0x3f);
				bits+=5;
				++it;
				++i;
				c<<=1;
			}
		}
		if (bits==6) bits++;
		code &= (1<<bits)-1;

		return code;
	}
	int shift(){
		return shift(it);
	}

	bool eof(){return it==end;}
};


template<typename T>
class StringIterator_EUC{
	friend StringIteratorProxy< StringIterator_EUC >;
	T it,end;
public:
	typedef T iterator;
	StringIterator_EUC(const T &begin_,const T &end_):it(begin_),end(end_) {};
	int shift(T &it){
		int c = (unsigned char)*it;
		if( c&0x80 )
			c = (unsigned char)*(++it) | c<<8;
		ir++;
		return c&0x7f7f;
	}
	int shift(){
		return shift(it);
	}

	bool eof(){return it==end;}
};

template<typename T>
class StringIterator_JIS{
	friend StringIteratorProxy< StringIterator_JIS >;
	T it,end;
	int jismode;
public:
	typedef T iterator;
	StringIterator_JIS(const T &begin_,const T &end_):it(begin_),end(end_) {};
	int shift(T &it){

		if (*tmp==0x1b) { // KI/KO
			if (*++it=='$' && *++it=='B') {jismode=1;++it;}
			if (*++it=='(' && *++it=='B') {jismode=0;++it;}
		}


		int c = (unsigned char)*it;
		if( jismode )
			c = (unsigned char)*(++it) | c<<8;
		return c;
	}
	int shift(){
		return shift(it);
	}

	bool eof(){return it==end;}
};


typedef StringIteratorProxy< StringIterator_SJIS<  std::string::iterator > > SJIS_IteratorS;
typedef StringIteratorProxy< StringIterator_SJIS<  char const* > > SJIS_IteratorC;
typedef StringIteratorProxy< StringIterator_UTF8<  std::string::iterator > > UTF8_IteratorS;
typedef StringIteratorProxy< StringIterator_UTF8<  char const* > > UTF8_IteratorC;

template<typename T>
std::string& stringEncode_SJIS(std::string &s,T begin,T end)
{
	s.clear();
	for(;begin!=end;++begin) {
		if (*begin>0xff)
			s.push_back(*begin>>8);
		s.push_back(*begin);
	}
	return s;
}


template<typename T>
std::string& stringEncode_UTF8(std::string &s,T begin,T end)
{
	s.clear();
	for(;begin!=end;++begin) {
		short wc = *begin;
		if (wc<=0x7f) {s.push_back((char)wc); continue; }
		unsigned char m = 0x1f;
		unsigned char m2=0x7f;
		int i=0;
		do {
			i++;
			m>>=1;
			m2>>=1;
		}while (m && m < (wc>>6*i));
		int t=i+1;
		s.push_back((char)(wc>>6*i) | ~m2);
		do {
			i--;
			s.push_back((char)(wc>>6*i)&0x3f | 0x80);
		} while(i);

	}
	return s;
}

}
#endif
