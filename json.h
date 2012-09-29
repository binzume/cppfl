/*
 * simple json parser  for C++
 *    http://www.binzume.net/software/cppfl/
 */
#ifndef CPPFL_JSON_H
#define CPPFL_JSON_H
#include <string>
#include <vector>
#include <map>

namespace json {

struct jstype {
	enum enum_t{
		null,
		boolean,
		number,
		string,
		array,
		object,
	};
	enum_t value;
	jstype():value(null){}
	jstype(enum_t v):value(v){}
	operator enum_t() const {return value;}
};

template <typename T> struct static_data {
  static T nullobj;
};
template <typename T> T static_data<T>::nullobj;

class jsvalue {
public:
	jstype type;
	long value_int;
	std::string value_string;
	std::map<std::string,jsvalue> value_object;
	std::vector<jsvalue> value_array;

	jsvalue():value_int(0){}
	jsvalue& operator [](const std::string &k) {
		if (type != jstype::object)
			return static_data<jsvalue>::nullobj;
		return value_object[k];
	}
	jsvalue& operator [](const char *k) {
		return (*this)[std::string(k)];
	}
	jsvalue& operator [](int k) {
		if (type != jstype::array || k>=value_array.size())
			return static_data<jsvalue>::nullobj;
		return value_array[k];
	}
	size_t size(){
		return (type==jstype::object)?value_object.size():value_array.size();
	}
	
	bool is_null() { return type == jstype::null; }
	bool is_array() { return type == jstype::array; }
	bool is_object() { return type == jstype::object; }
	bool is_string() { return type == jstype::string; }
	bool is_number() { return type == jstype::number; }
	const std::vector<jsvalue>& get_array() const { return value_array; }
	const std::map<std::string,jsvalue>& get_object() const { return value_object; }

	std::string to_s() {
		return value_string;
	}
	long to_i(){
		return value_int;
	}
	operator int(){return to_i();}
	operator std::string(){return to_s();}
};

	template <typename IT>
	bool parse_strv(std::string &v, IT &it, const IT &end){
		++it; // "
		for (;*it!='\"';++it) {
			if (it == end) return false;
			if (*it == '\\') {
				if (++it == end) return false;
				if (*it == 'n') {
					v.append("\n");
					continue;
				}
			}
			v+=*it;
		}
		++it;
		return true;
	}

	template <typename IT>
	void parse_intv(long &v, IT &it, const IT &end){
		int sign = 1;
		if (*it == '-') {
			sign = -1;
			++it;
		} else if (*it == '+') ++it;
		for (v=0; it != end && isdigit(*it); ++it) v = v*10 + *it-'0';
		v *= sign;
	}

	template <typename IT>
	bool parse_str(jsvalue &node, IT &it, const IT &end){
		node.type = jstype::string;
		return parse_strv(node.value_string,it,end);
	}

	template <typename IT>
	bool parse_int(jsvalue &node, IT &it, const IT &end){
		node.type = jstype::number;
		parse_intv(node.value_int,it,end);
		if (*it == 'E' || *it=='e') {
			long d;
			parse_intv(d,++it,end);
			for (;d>0;d--) node.value_int *= 10;
		}
		return true;
	}

	template <typename IT>
	bool parse_array(jsvalue &node, IT &it, const IT &end){
		node.type = jstype::array;
		skip(++it,end);
		for (;*it != ']';++it) {
			node.value_array.push_back(jsvalue());
			if (!parse_(node.value_array.back(), it, end)) return false;
			if (*it == ']') break;
			if (*it != ',') return false;
		}
		++it; // ]
		return true;
	}

	template <typename IT>
	bool parse_obj(jsvalue &node, IT &it, const IT &end){
		node.type = jstype::object;
		skip(++it,end);
		for (;*it != '}';++it) {
			std::string name;
			skip(it,end);
			if (!parse_strv(name, it, end)) return false;
			skip(it,end);
			if (*it != ':') return false;
			if (!parse_(node.value_object[name], ++it, end)) return false;
			if (*it == '}') break;
			if (*it != ',') return false;
		}
		++it; // }
		return true;
	}

	template <typename IT>
	bool parse_word(jsvalue &node, IT &it, const IT &end){
		std::string w;
		for (;it != end && isalpha(*it);++it) w+=*it;
		if (w == "true") {
			node.type = jstype::boolean;
			node.value_int = 1;
		} else if (w == "false") {
			node.type = jstype::boolean;
			node.value_int = 0;
		} else {
			node.type = jstype::null;
			node.value_int = 0;
		}
		return it != end;
	}

	template <typename IT>
	void skip(IT &it, const IT &end) {
		while(it != end && isspace(*it)) ++it;
	}

	template <typename IT>
	bool parse_(jsvalue &node,IT &it, const IT &end) {
		skip(it,end);
		if (*it == '[')
			parse_array(node,it,end);
		else if (*it == '{')
			parse_obj(node,it,end);
		else if (*it == '\"' || *it == '\'')
			parse_str(node,it,end);
		else if (*it >= '0' && *it <= '9' || *it=='-')
			parse_int(node,it,end);
		else
			parse_word(node,it,end);
		skip(it,end);
		return true;
	}

	template <typename IT>
	bool parse(jsvalue &node, const IT &begin, const IT &end) {
		IT it=begin;
		return parse_(node,it,end);
	}

	bool parse(jsvalue &node,const std::string &str) {
		std::string::const_iterator it = str.begin();
		return parse_(node,it,str.end());
	}

};
#endif
