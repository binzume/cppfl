#include <string>
#include <vector>

#define STR std::string()+

std::string operator+(const std::string &s1,int i) {
	char s2[16];
	sprintf(s2,"%d",i);
    return s1+s2;
}
std::string operator+(int i,const std::string &s1) {
	char s2[16];
	sprintf(s2,"%d",i);
    return s2+s1;
}
std::string& operator+=(std::string &s1,int i) {
	char s2[16];
	sprintf(s2,"%d",i);
	s1+=s2;
    return s1;
}

std::string operator+(const std::string &s1,double i) {
	char s2[16];
	sprintf(s2,"%lf",i);
    return s1+s2;
}
std::string operator+(double i,const std::string &s1) {
	char s2[16];
	sprintf(s2,"%lf",i);
    return s2+s1;
}
std::string& operator+=(std::string &s1,double i) {
	char s2[16];
	sprintf(s2,"%lf",i);
	s1+=s2;
    return s1;
}

template<typename T>
void split(std::vector<T> &list, const std::string &sep,const T &str) {
	int p=0;
	while (p<str.size()) {
		int l=str.find(sep,p);
		if (l==T::npos) l=str.size();
		list.push_back(str.substr(p,l-p));
		p=l+1;
	}
}


inline std::vector<std::string> split(const std::string &sep,const std::string &src) {
	std::vector< std::string > list;
	split(list,sep,src);
	return list;
}


inline std::string& replace(std::string &dst, const std::string &src, const std::string &from, const std::string &to) {
	dst = src;
	for (std::string::size_type p=0;(p=dst.find(from,p))!=std::string::npos;p+=to.size() ) {
	    dst.replace(p, from.size(), to);
	}
    return dst;
}

inline std::string replace(const std::string &src, const std::string &from, const std::string &to) {
	std::string s;
	return replace(s,src,from,to);
}

class _split {
public:
	const std::string str;
	_split(const std::string &s):str(s){}
};

class _replace {
public:
	const std::string s1;
	const std::string s2;
	_replace(const std::string &from,const std::string &to):s1(from),s2(to){}
};


std::vector<std::string> operator *(const std::string &s,_split &sp) {
	return split(sp.str,s);
}
std::string operator *(const std::string &s,_replace &sp) {
	return replace(s,sp.s1,sp.s2);
}

inline _split split(const std::string &s) {
	return _split(s);
}

inline _replace replace(const std::string &from,const std::string &to) {
	return _replace(from,to);
}

