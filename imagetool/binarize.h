#ifndef _IMAGE_BINARIZE_H
#define _IMAGE_BINARIZE_H

namespace imagetool
{

static inline void thresholdRGB(Bitmap &dst,const Bitmap &src,int th)
{
	for (scan_iterator it(src);it;++it) {
		dst.set(it.x,it.y,(*it).getv()<th?255:0);
	}
}

static inline void _testRGB(Bitmap &dst,const Bitmap &src,int th)
{
	 int dx[] = {-1,0,1, -1,1, -1,0,1}, dy[] = {-1,-1,-1, 0,0, 1,1,1};
	int mode = 1;
	for (scan_iterator it(src);it;++it) {
		int v = (*it).getv();
		if (it.is_edge() || v>th+10 || v<th-10) {
			mode = v<th;
		} else {
			int min = 255, max = 0;
			for (int i=0; i<sizeof(dx)/sizeof(int); i++) {
				int t = const_cast<Bitmap&>(src)(it.x+dx[i],it.y+dy[i]).getv();
				if (t>max) max=t;
				if (t<min) min=t;
			}
			if (max-min>10) {
				mode = v<th;
			}
		}
		dst.set(it.x,it.y,mode?255:0 | ((*it).value)&0xffff00);
	}
}
 

}

#endif

