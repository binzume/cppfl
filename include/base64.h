/***************************************************************************
 *
 *            Base64 Encode/Decode
 *
 *
 *
 *                               Copyright(C) 2004 kstm.org @ sinshu-univ.
 ***************************************************************************/
#ifndef _BASE64_H
#define _BASE64_H

#include <string>

namespace base64{

template <typename T>
void encode(std::string &dst, T it,const T end) {
	int n , b = 0, d = 0, p = 0;
	dst.clear();
	for (;;) {
		if (b < 6 && it!=end) {
			p++;
			d = (d<<8) + (unsigned char)*it;
			++it;
			b += 8;
		}
		if (b<=0) break;
		n = ((d<<6)>>b)&0x3f;
		b -= 6;
		if (n<26) {
			n += 'A';
		} else if (n<52) {
			n += 'a'-26;
		} else if (n<62) {
			n += '0'-52;
		} else if (n==62) {
			n = '+';
		} else { // (n==63)
			n = '/';
		}
		dst.push_back((char)n);
	}
	while (dst.size()&3) dst.push_back('=');; // パディング
}
void encode(std::string &dst, const std::string &src) {
	encode(dst,src.begin(),src.end());
}

std::string encode(const std::string &src) {
	std::string dst;
	encode(dst,src);
	return dst;
}

template <typename T>
void decode(std::string &dst, T it,const T end) {
	int b = 0 , d = 0;
	while (it != end) {
		if (*it>='A' && *it<='Z') {
			d = (d<<6) + *it-'A';
		} else if (*it>='a' && *it<='z') {
			d = (d<<6) + *it-'a'+26;
		} else if (*it>='0' && *it<='9') {
			d = (d<<6) + *it-'0'+52;
		} else if (*it=='+') {
			d = (d<<6) + 62;
		} else if (*it=='/') {
			d = (d<<6) + 63;
		} else {
			++it;
			continue;
		}
		b += 6;
		++it;
		if (b>=8) {
			b -= 8;
			dst.push_back((char)(d>>b));
		}
	}
}

void decode(std::string &dst, const std::string &src) {
	decode(dst,src.begin(),src.end());
}

std::string decode(const std::string &src) {
	std::string dst;
	decode(dst,src);
	return dst;
}

}
#endif
