/***************************************************************************
 *
 *             Byte order
 *
 *
 *
 *                                   Copyright(C) 2004 kstm @ Sinshu univ.
 ***************************************************************************/

 //バイトオーダー
#ifndef BYTE_ORDER
#  define LITTLE_ENDIAN 1234
#  define BIG_ENDIAN    4321
#  if defined(i386) || defined(_WIN32)
#     define BYTE_ORDER LITTLE_ENDIAN
#  elif __APPLE__
#    if __BIG_ENDIAN
#      define BYTE_ORDER BIG_ENDIAN
#    else
#      define BYTE_ORDER LITTLE_ENDIAN
#    endif
#  endif
#endif

#ifndef BYTE_ORDER
#	pragma message("BYTE_ORDER is not defined! (LITTLE_ENDIAN mode)")
#	define BYTE_ORDER LITTLE_ENDIAN
#endif


#if BYTE_ORDER==LITTLE_ENDIAN
#	define LE(v) v
#	define LES(v) v
#endif

#if BYTE_ORDER==BIG_ENDIAN
//#	define toLE(v) ((v&0xff) << 24 | (v&0xff00) << 8 | (v&0xff0000) >> 8 | (unsigned)v >> 24)
	inline unsigned long LE(unsigned long x){
		//x=_rotl(x,16);
		x = x >> 16 | x << 16;
		return ((x & 0xff00ff00) >> 8) | ((x&0x00ff00ff) << 8);
	}
	inline unsigned short LES(unsigned short x){
		return (x>>8) | (x<<8);
	}
#endif

#if BYTE_ORDER==PDP_ENDIAN // PDP Endian
	inline unsigned long LE(unsigned long x){
		return x >> 16 | x << 16;
	}
#	define LES(v) v
#endif
#endif

