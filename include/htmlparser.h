
#include <string>
#include <vector>
#include <map>
#include "encode.h"

namespace HTML{

typedef wchar_t htmlchar;
typedef std::basic_string<wchar_t> htmlstring;
typedef std::vector<class Element*> NodeList;
//typedef std::basic_string<char> htmlstring;
static const int NOTE_TYPE_TEXT = 3;
static const int NOTE_TYPE_DOCUMENT = 9;


static htmlstring& htmlunescape(htmlstring &dst, const htmlstring &src) {
	static std::map<std::string,htmlchar> reftable;
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
	int len=src.size();
	
	for (int i=0;i<src.size();i++) {
		int c = src[i];
		if (c=='&') {
			int j;
			i++;
			for(j=0; i+j<src.size() && src[i+j]!=';' && j<8; j++) {
				name[j] = src[i+j];
			}
			if (src[i+j]==';') {
				i+=j;
				name[j] = '\0';
				if (reftable.count(name))
					c = reftable[name];
			}
		}
		dst.push_back(c);
	}

	return dst;
}



class Element {
protected:
public:
	std::string tagName;
	class Container *parent;
	short nodeType;

	Element(const std::string &tag){
		tagName = tag;
		//cout << tagName << endl;
		nodeType = 0;
		parent=NULL;
	}

	void setTagName(std::string t){tagName=t;}
	std::string getTagName(){return tagName;}
	short getNodeType(){return nodeType;}
	virtual void setAttributes(std::map<std::string,std::string> &attrs){}
	bool isContainer(){return nodeType==1 || nodeType==9;}
	virtual htmlstring& getInnerText(htmlstring &s)=0;
	virtual htmlstring getInnerText(){htmlstring s;return getInnerText(s);}
	virtual ~Element(){}

#ifndef WITHOUT_ATTRIBUTE
	virtual NodeList& getElementsByAttribute(NodeList &list,const std::string &name,const std::string &value) {return list;}
	virtual NodeList& getElementsByName(NodeList &list,const std::string &name){return list;}
	virtual NodeList& getElementsByClassName(NodeList &list,const std::string &name){return list;}
#endif

};

class Container : public Element {
public:
	std::vector<Element*> childNodes;
public:

	Container(const std::string &tag) : Element(tag){
		nodeType = 1;
	}

	void appendChild(Element *e){e->parent=this;childNodes.push_back(e);}

	void clear(){
		unsigned int i;
		for (i=0;i<childNodes.size();i++) {
			delete childNodes[i];
		}
		childNodes.clear();
	}

#ifndef WITHOUT_ATTRIBUTE
	std::map<std::string,std::string> attributes;
	void setAttributes(std::map<std::string,std::string> &attrs){attributes.swap(attrs);}


	// NOT IMPLEMENTED
	NodeList& getElementsByAttribute(NodeList &list,const std::string &name,const std::string &value){
		if (attributes.count(name)) {
			if (attributes[name]==value) list.push_back(this);
		}
		for (unsigned int i=0;i<childNodes.size();i++) {
			if (childNodes[i]->isContainer()) ((Container*)childNodes[i])->getElementsByAttribute(list,name,value);
		}
		return list;
	}

	NodeList& getElementsByName(NodeList &list,const std::string &name){
		list.clear();
		return getElementsByAttribute(list,"name",name);
	}
	NodeList& getElementsByClassName(NodeList &list,const std::string &name){
		list.clear();
		return getElementsByAttribute(list,"class",name);
	}

#endif

	NodeList& getElementsByTagName(NodeList &list,const std::string &s){
		for (unsigned int i=0;i<childNodes.size();i++) {
			if (childNodes[i]->tagName == s) list.push_back(childNodes[i]);
			if (childNodes[i]->isContainer()) ((Container*)childNodes[i])->getElementsByTagName(list,s);
		}
		return list;
	}

	htmlstring& getInnerText(htmlstring &s) {
		for (unsigned int i=0;i<childNodes.size();i++) {
			childNodes[i]->getInnerText(s);
		}
		return s;
	}

	virtual ~Container(){
		clear();
	}

};



class TextNode :public Element{
public:
	htmlstring text;
	TextNode(htmlstring &str, bool fswap = false) : Element("_text"){
		if (fswap) {
			text.swap(str);
		} else {
			text=str;
		}
		//cout << text << endl;
		nodeType = 3;
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
	std::map<std::string,int> autoclose;
	htmlstring title;
	Encode::ENCODE encode;

	Document() : Container("_document"){
		height=0;
		nodeType = 9;
		const char *ct[] = {"br","img","hr","meta","link"};
		for (int i=0;i<sizeof(ct)/sizeof(const char*);++i) {
			autoclose[ct[i]] = 1;
		}
	}

	Element* getElementById(const std::string &id) {
		return idmap.count(id)?idmap[id]:NULL;
	}

	Element* createElement(const std::string &name) {
		return new Container(name);
	}

	int parse(const std::string &s, Encode::ENCODE enc = Encode::AUTO) {
		return parse(s.c_str(),s.length(),enc);
	}
	int parse(const char *html,int size, Encode::ENCODE enc = Encode::AUTO) {
		encode = enc;
		if (enc == Encode::UTF8) { // FIXME!
			Encode::UTF8_IteratorC it(html,html+size);
			return parse(it);
		}
		Encode::SJIS_IteratorC it(html,html+size);
		return parse(it);
	}

	template<typename T>
	int parse(T it) {
		title = L"no title";

		Container *n=this;
		int i=0,st=0,f=0;
		bool sf=true;
		htmlstring ws;
		htmlchar wc;

		char s[40];
		while(!it.eof()) {
			wc=*it;
			it++;

			if (f==0 && wc=='<') {
				if (st) {
					if (n->tagName=="title") title = ws;
					htmlstring tmp;
					n->appendChild(new TextNode(htmlunescape(tmp,ws),true));
					ws.clear();
					st=0;
				}
				int tagclose=0;
				if (*it=='/') {
					tagclose=1;
					++it;
				}
				while(!it.eof() && *it<0x100&&isalnum(*it) && st<32) {s[st++]=*it++|0x20;}
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
					while(!it.eof() && *it!='>') ++it;
					if (!it.eof()) ++it;
				} else if (st) {
					s[st]='\0';
					std::map<std::string,std::string> attr;
					getAttr(attr,it);
					while(!it.eof() && *it!='>') tagclose=*it=='/',++it;
					if (!it.eof()) ++it;

					Element *e = createElement(s);
					//Container *t=new Container(s);
					if (attr.count("id")) idmap[attr["id"]] = e;
					e->setAttributes(attr);
					n->appendChild(e);
					if (!tagclose && e->isContainer()) n=(Container*)e;
				}
				st=0;
			} else {
				if (st || wc>0x80 || !isspace(wc)) {
					if (f==0 && wc<128 && isspace(wc)) {
						if (!sf) ws.append(1,' ');
						sf=true;
					} else {
						if (sizeof(htmlstring::value_type)<sizeof(wchar_t)) {
							if (wc>=0x100)
								ws.append(1,wc>>8);
						}
						ws.append(1,wc);
						sf=false;
					}
					st++;
				}
			}

		}
		if (st) {
			htmlstring tmp;
			n->appendChild(new TextNode(htmlunescape(tmp,ws),true));
			st=0;
		}
		return 1;
	}

	template <typename TT>
	void getAttr(std::map<std::string,std::string> &attr,TT it){
		//while(it!=end && !isspace((unsigned char)*it)) ++it;
		std::string k,v;
		bool f=0;
		while(!it.eof() && *it!='>') {
			if (*it=='=') {
				f=true;
			} else if (!isspace((unsigned char)*it)){
				if (f) {
					v="";
					if (*it=='\"' || *it=='\'') {
						char q=*it;++it;
						while(!it.eof() && *it!=q) {
							int d;
							v.append(1,*it);
							++it;
						}
						if (!it.eof()) ++it;
					} else if (*it<0x100&&isalnum(*it)){
						while(!it.eof() && (!isspace((char)*it)&&*it!='>')) v.append(1,*it),++it;
					} else {
						++it;
					}
					f=false;
					attr[k]=v;
					k="";
				} else {
					if (*it<0x100&&isalnum(*it)){
						while(!it.eof() && (*it<0x100&&isalnum(*it)||*it=='-')) k.append(1,*it++|0x20);
					} else {
						++it;
					}
				}
				continue;
			}
			++it;
		}
	}

};

}
