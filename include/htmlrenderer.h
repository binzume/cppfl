
#include <string>
#include <vector>
#include <map>

namespace HTML{



static std::string &htmlunescape(std::string &dst, const std::string &src) {
	static std::map<std::string,wchar_t> reftable;
	if (reftable.size()==0) {
			reftable["quot"]='\"';
			reftable["amp"]='&';
			reftable["lt"]='<';
			reftable["gt"]='>';
			reftable["nbsp"]=' ';
			reftable["copy"]=0xA9;
			reftable["reg"]=0xAE;
			reftable["dagger"]=0x2020;
			reftable["larr"]=0x2190;
			reftable["uarr"]=0x2191;
			reftable["rarr"]=0x2192;
			reftable["darr"]=0x2193;
			//reftable["crarr"]=0x21B5;
	}
	char name[8];

	dst.clear();
	
	for (int i=0;i<src.size();i++) {
	}

	return dst;
}


template<typename T>
class SJIS_Iterator{
public:
	T it;
	typedef T ittype;
	SJIS_Iterator(const T &s):it(s){}
	int operator *(){
		int c = (unsigned char)*it;
		if( ((unsigned char)((c^0x20)-0xA1)<=0x3B) )
			c |= (unsigned char)*(it+1)<<8;
		return c;
	}
	SJIS_Iterator& operator ++(){
		if( ((unsigned char)((*it^0x20)-0xA1)<=0x3B) )
			++it;
		++it;
		return *this;
	}
	SJIS_Iterator operator ++(int){
		SJIS_Iterator<T> tmp=*this;
		++(*this);
		return tmp;
	}
	bool operator !=(const T &s){
		return it!=s;
	}
	bool operator !=(const SJIS_Iterator &s){
		return it!=s.it;
	}
};

template<typename T>
class StringIteratorT :public SJIS_Iterator<T>{
public:
	StringIteratorT(const T &s):SJIS_Iterator(s){}

};


typedef std::string htmlstring;
typedef int htmlchar;
typedef StringIteratorT<  std::string::iterator > StringIterator;
typedef StringIteratorT< char const* > StringIterator2;

class Screen{
public:
	int width,height;
	virtual void color(int c)=0;
	virtual void boxf(int x,int y,int w,int h)=0;
	virtual void line(int x,int y,int w,int h)=0;
	virtual void draw_char(int c,int x,int y)=0;
};

struct Font{
	Font(int h):height(h),color(-1),style(0){}
	int height;
	int width(htmlchar s){return (s>0xff)?height:(height/2);}
	int color,style;
} font(16);


struct Style{
	short color;
	short bgcolor;
	short font_style; // 1:UL 2:B
	short fblock;
	short margin[4]; // t,r,b,l
	short text_indent;
	short border_color;
};

#define RGB15(r,g,b) (r+g+b)

struct TAGDEF{
	char tag[8];
	Style style;
	int autoclose;
};

TAGDEF default_style[]={
	{"",{-1,-1,0,0,{0,0,0,0},0,-1},2},
	{"h1",{RGB15(10,0,0),RGB15(29,29,29),2,1,{5,0,4,0},0,-1},0},
	{"h2",{RGB15(10,0,0),RGB15(29,29,29),2,1,{5,0,4,0},0,-1},0},
	{"h3",{RGB15(10,0,0),RGB15(29,29,29),2,1,{5,0,4,0},0,-1},0},
	{"h4",{RGB15(10,0,0),RGB15(29,29,29),2,1,{5,0,4,0},0,-1},0},
	{"h5",{RGB15(10,0,0),RGB15(29,29,29),2,1,{5,0,4,0},0,-1},0},
	{"h6",{RGB15(10,0,0),RGB15(29,29,29),2,1,{5,0,4,0},0,-1},0},
	{"dd",{-1,-1,0,1,{0,0,0,10},0,-1},0},
	{"strong",{-1,-1,2,0,{0,0,0,0},0,-1},0},
	{"a",{RGB15(0,0,29),-1,1,0,{0,0,0,0},0,-1},0},
	{"tr",{-1,-1,0,1,{0,0,0,0},0,-1},0},
	{"ul",{-1,-1,0,1,{0,0,0,10},0,-1},0},
	{"ol",{-1,-1,0,1,{0,0,0,10},0,-1},0},
	{"div",{-1,-1,0,1,{0,0,0,0},0,-1},0},
	{"p",{-1,-1,0,1,{3,0,3,0},8,-1},0},
	{"title",{-1,-1,0,2,{0,0,0,0},0,-1},0}, // hide
	{"style",{-1,-1,0,2,{0,0,0,0},0,-1},0}, // hide
	{"script",{-1,-1,0,2,{0,0,0,0},0,-1},0}, // hide
	{"meta",{-1,-1,0,2,{0,0,0,0},0,-1},1},
	{"link",{-1,-1,0,2,{0,0,0,0},0,-1},1},
	{"br",{-1,-1,0,1,{0,0,0,0},0,-1},1},
	{"img",{-1,-1,0,2,{0,0,0,0},0,-1},1},
};





struct Layout{
	int x, y;
	int width,height;
	int line_height;
	int lh;
	int cx,ex;
	inline void newLine() {
		y+=lh;
		lh = line_height;
		cx = x;
	}
};
/*
  xy+ cx+----------------------+
        |                      |
    +---+                      |
    |                          |
    |             ex+----------+
    |               |
    +---------------+
    <-------------------------->
                  width
*/

class Element {
protected:
public:
	std::string tagName;
	class Container *parent;
	Layout lo;
	bool fcontainer;

	Element(const std::string &tag){
		tagName = tag;
		//cout << tagName << endl;
		fcontainer=false;
		parent=NULL;
	}

	void setTagName(std::string t){tagName=t;}
	std::string getTagName(){return tagName;}
	bool isContainer(){return fcontainer;}
	virtual void draw(Screen *b,int x,int y){}
	virtual void layout(Layout &l, Style &s)=0;
	virtual bool getElementsByPoint(std::vector<Element*> &list,int x,int y){return false;}
	virtual htmlstring& getInnerText(htmlstring &s)=0;
	virtual htmlstring getInnerText(){htmlstring s;return getInnerText(s);}
	virtual ~Element(){}
};

class Container : public Element {
public:
	std::vector<Element*> childNodes;
public:

	Container(const std::string &tag) : Element(tag){
		fcontainer=true;
	}

	void appendChild(Element *e){e->parent=this;childNodes.push_back(e);}

	void clear(){
		unsigned int i;
		for (i=0;i<childNodes.size();i++) {
			delete childNodes[i];
		}
		childNodes.clear();
	}
	void getElementsByTagName(std::vector<Element*> &list,const std::string &s){
		for (unsigned int i=0;i<childNodes.size();i++) {
			if (childNodes[i]->tagName == s) list.push_back(childNodes[i]);
			if (childNodes[i]->fcontainer) ((Container*)childNodes[i])->getElementsByTagName(list,s);
		}
	}
	bool getElementsByPoint(std::vector<Element*> &list,int x,int y){
		bool f=false;
		for (unsigned int i=0;i<childNodes.size();i++) {
			f=childNodes[i]->getElementsByPoint(list,x,y) || f;
		}
		if (f) list.push_back(this);
		return f;
	}
	htmlstring& getInnerText(htmlstring &s) {
		for (unsigned int i=0;i<childNodes.size();i++) {
			childNodes[i]->getInnerText(s);
		}
		return s;
	}

	void layout(Layout &l, Style &s){
		unsigned int i;
		for (i=0;i<childNodes.size();i++) {
			childNodes[i]->layout(l,s);
		}
	}
	void draw(Screen *scr,int x,int y){
		unsigned int i;
		for (i=0;i<childNodes.size();i++) {
			childNodes[i]->draw(scr,x,y);
		}
	}
	virtual ~Container(){
		clear();
	}

};

class DisplayObject :public Container{
public:
	Style *style;
	DisplayObject(const std::string &t,Style *s): Container(t){
		style=s;
	}
	void draw(Screen *b,int x,int y){
		if (style->fblock==2) return;
		if (lo.y+lo.height+y > 0 && lo.y+y < b->height) {
			if (style->fblock && style->bgcolor>=0) {
				b->color(style->bgcolor);
				b->boxf(x+lo.x,y+lo.y,b->width-(x+lo.x),lo.height);
			}
			if (style->color>=0) {
					b->color(style->color);
			}
		}
		Container::draw(b,x,y);
	}
	void layout(Layout &l, Style &s){
		if (style->fblock==2) return;
		if (style->fblock) {
			if (l.cx!=l.x) {
				l.newLine();
			}
			l.y += style->margin[0];
			l.x += style->margin[3];
			l.cx = l.x+style->text_indent;
		}
		lo = l;

		Style s1 = s;
		if (style->font_style!=-1) s1.font_style = style->font_style;
		if (style->color!=-1) s1.color = style->color;

		Container::layout(l,s1);

		if (style->fblock) {
			if (l.cx!=l.x) {
				l.newLine();
				l.x = lo.x;
			}
			lo.height = l.y-lo.y;
			l.y += style->margin[2];
			l.x -= style->margin[3];
		} else {
			lo.height = l.y+l.lh-lo.y;
		}
		lo.ex = l.cx;
	}
};


class TextNode :public Element{
	int color,style;
public:
	htmlstring text;
	TextNode(htmlstring &str, bool fswap = false) : Element("_text"){
		color=-1;
		style=0;
		if (fswap) {
			text.swap(str);
		} else {
			text=str;
		}
		//cout << text << endl;
	}
	void draw(Screen *b,int x,int y){
		x+=lo.cx;
		y+=lo.y;
		int c=color;
		if (c>=0) b->color(c);

		for (StringIterator it=text.begin(); it!=text.end(); ++it) {
			c = *it;
			int w = font.width(c);
			if (x+w>lo.width) {
				x=lo.x;
				y+=lo.line_height;
			}
			if (y>-font.height && y<b->height) {
				if (style&1) b->line(x,y+font.height,x+w+1,y+font.height);
				if (style&2) b->draw_char(c,x,y);
				b->draw_char(c,x,y);
			}
			x+=w+1;
		}
	}
	bool getElementsByPoint(std::vector<Element*> &list,int px,int py){
		if (py<lo.y || py>lo.height) return false;
		int xx=lo.cx,yy=lo.y;
		for (StringIterator it=text.begin(); it!=text.end(); ++it) {
			int w=font.width(*it);
			if (xx+w>=lo.width) {
				xx=lo.x;
				yy+=lo.line_height;
			}
			if (py>=yy && py<yy+font.height && px>=xx && px<xx+w) return true;
			xx+=w;
		}
		return false;
	}
	void layout(Layout &l, Style &s){
		lo = l;
		lo.y = l.y+l.lh-font.height;
		style=s.font_style;
		color=s.color;

		for (StringIterator it=text.begin(); it!=text.end(); ++it) {
			int w=font.width(*it);
			if (l.cx+w>l.width) {
				l.newLine();
			}
			l.cx+=w+1;
		}
		lo.ex = l.cx;
		lo.height = l.y+l.lh-lo.y;
	}
	htmlstring& getInnerText(htmlstring &s) {
		s+=text;
		return s;
	}
};

class Document :public Container{
public:
	int height;
	std::map<std::string,Element*> idmap;
	std::map<std::string,Style*> style;
	std::map<std::string,int> autoclose;
	htmlstring title;

	Document() : Container("_document"){
		height=0;
		for (unsigned int i=0;i<sizeof(default_style)/sizeof(TAGDEF);i++) {
			style[default_style[i].tag]=&default_style[i].style;
			if (default_style[i].autoclose) autoclose[default_style[i].tag] = default_style[i].autoclose;
		}
	}

	Element* getElementById(const std::string &id) {
		return idmap.count(id)?idmap[id]:NULL;
	}

	Element* createElement(const std::string &name) {
		return new DisplayObject(name,style[""]);
	}

	int parse(const char *html,int size) {
		title = "no title";
/*
		if ( ImageLoader::check(html,size) ) {
			HTML_IMG *img = new TagIMG();
			ImageLoader::load(img->img,html,size);
			this->appendChild(img);
			return 1;
		}
		Encode::select(force_charset?charset:0);
*/
		Container *n=this;
		int i=0,st=0,f=0;
		bool sf=true;
		htmlstring ws;
		htmlchar wc;

		char s[40];
		StringIterator2 it = html;
		StringIterator2 end=html+size;
		while(it!=end) {
			wc=*it;
			it++;

			if (f==0 && wc=='<') {
				if (st) {
					if (n->tagName=="title") title = ws;
					n->appendChild(new TextNode(ws,true));
					st=0;
				}
				int tagclose=0;
				if (*it=='/') {
					tagclose=1;
					++it;
				}
				while(it!=end && *it<0x100&&isalnum(*it) && st<32) {s[st++]=*it++|0x20;}
				if (st && tagclose) {
					s[st]='\0';
					Element *t=n;
					while(t){
						if (t->tagName==s) {
							n=(Container*)t;
							if (n->parent) n = n->parent;
							break;
						}
						t=t->parent;
					}
					while(it!=end && *it!='>') ++it;
					if (it!=end) ++it;
				} else if (st) {
					s[st]='\0';
					std::map<std::string,std::string> attr;
					getAttr(attr,it,end);
					while(it!=end && *it!='>') tagclose=*it=='/',++it;
					if (it!=end) ++it;
					if (style.count(s)) {
						DisplayObject *t=new DisplayObject(s,style[s]);
						if (attr.count("id")) idmap[attr["id"]] = t;
						n->appendChild(t);
						if (tagclose==0 && (autoclose.count(s)==0 || autoclose[s] !=1)) n=t;
/*
					} else if (strcmp(s,"hr")==0) {
						n->add(new HTML_hr());
					} else if (strcmp(s,"a")==0) {
						HTML_A *t=new HTML_A();
						if (attr.count("href"))
							t->setHref(attr["href"]);
						n->add(t);
						n=t;
					} else if (strcmp(s,"img")==0) {
						HTML_IMG *t=new HTML_IMG();
						t->src=attr["src"];
						n->add(t);
					} else if (strcmp(s,"input")==0) {
						HTML_FormItem *t=new HTML_FormItem(s,NULL);
						t->setattr(attr);
						n->add(t);
					} else if (strcmp(s,"form")==0) {
						HTML_FORM *t=new HTML_FORM(s,style["div"]);
						t->setattr(attr);
						n->add(t);
						n=t;
					} else if (strcmp(s,"li")==0) {
						HTMLContainer *t=n;
						while(t){
							if (t->tag=="ul" || t->tag=="ol") {
								n=t;
								break;
							}
							t = t->parent;
						}
						t=new HTML_LI();
						n->add(t);
						n=t;
					} else if (strcmp(s,"title")==0) {
						HTMLContainer *t=new HTML_TITLE();
						n->add(t);
						n=t;
					} else if (strcmp(s,"meta")==0) {
						if (force_charset==0 && attr.count("content")) {
							if (strstr(attr["content"].c_str(),"UTF")){
								Encode::select(0);
							} else if (strstr(attr["content"].c_str(),"EUC") || strstr(attr["content"].c_str(),"euc")){
								Encode::select(1);
							} else if (strstr(attr["content"].c_str(),"hift") || strstr(attr["content"].c_str(),"sjis")){
								Encode::select(2);
							} else if (strstr(attr["content"].c_str(),"-2022-")){
								Encode::select(3);
							}
						}
*/
					} else {
						DisplayObject *t=new DisplayObject(s,style[""]);
						if (attr.count("id")) idmap[attr["id"]] = t;
						n->appendChild(t);
						if (!tagclose) n=t;
					}
				}
				st=0;
			} else {
				if (st || wc>0x80 || !isspace(wc)) {
					if (f==0 && wc<128 && isspace(wc)) {
						if (!sf) ws.append(1,' ');
						sf=true;
					} else {
						ws.append(1,wc);
						if (wc>=0x100)
							ws.append(1,wc>>8);
						sf=false;
					}
					st++;
				}
			}

		}
		if (st) {
			n->appendChild(new TextNode(ws,true));
			st=0;
		}
		layout();
		return 1;
	}

	template <typename TT>
	void getAttr(std::map<std::string,std::string> &attr,StringIteratorT<TT> it,const StringIteratorT<TT> &end){
		while(it!=end && !isspace((unsigned char)*it)) ++it;
		std::string k,v;
		bool f=0;
		while(it!=end && *it!='>') {
			if (*it=='=') {
				f=true;
			} else if (!isspace((unsigned char)*it)){
				if (f) {
					v="";
					if (*it=='\"' || *it=='\'') {
						char q=*it;++it;
						while(it!=end && *it!=q) {
							int d;
							v.append(1,*it);
							++it;
						}
						if (it!=end) ++it;
					} else if (*it<0x100&&isalnum(*it)){
						while(it!=end && (!isspace((char)*it)&&*it!='>')) v.append(1,*it),++it;
					} else {
						++it;
					}
					f=false;
					attr[k]=v;
					k="";
				} else {
					if (*it<0x100&&isalnum(*it)){
						while(it!=end && (*it<0x100&&isalnum(*it)||*it=='-')) k.append(1,*it++|0x20);
					} else {
						++it;
					}
				}
				continue;
			}
			++it;
		}
	}

//*/
	void layout(int width=256){
		Layout l = {0,0,width,0,font.height,font.height,0,0};
		Style s = *style[""];
		lo=l;
		Container::layout(l,s);
		lo.height=l.y+l.lh;
		height=lo.height;
	}

};

}
