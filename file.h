#ifndef _FILE_H
#define _FILE_H

#include <string>
#include <vector>


#ifdef _WIN32
// dirent.h for Win32 ?
#include <windows.h>
#define DT_DIR FILE_ATTRIBUTE_DIRECTORY
#ifndef MAX_PATH
#define MAX_PATH 512
#endif
struct dirent{
	int d_ino;
	int d_type;
	char d_name[256];
};
struct DIR{
	char path[MAX_PATH];
	dirent dir;
	WIN32_FIND_DATA w32FindData;
	HANDLE hFile;
};

static DIR* opendir(const char *path){
	DIR *d = new DIR;
	strcpy(d->path,path);
	strcat(d->path,"\\*");
	d->hFile = FindFirstFile(d->path,&d->w32FindData);
	if (d->hFile == INVALID_HANDLE_VALUE) {
		d->hFile = NULL;
		return NULL;
	}
	return d;
}

static int closedir(DIR *d){
	if(!d) return 0;
	if (d->hFile) FindClose(d->hFile);
	delete d;
	return 1;
}

static dirent* readdir(DIR *d){
	if (!d->hFile) return NULL;
	d->dir.d_type=d->w32FindData.dwFileAttributes;
	d->dir.d_ino=1;
	strcpy(d->dir.d_name,d->w32FindData.cFileName);
	if(!FindNextFile(d->hFile,&d->w32FindData)){
		FindClose(d->hFile);
		d->hFile=NULL;
	}
	return &d->dir;
}
#else
#include <dirent.h>
#endif


class File{
public:
	//std::string path;
	FILE *fp;
	File(const std::string &fname,char *mode="rb") {
		fp=fopen(fname.c_str(),mode);
	}
	~File(){close();}
	template<typename T> T* load(){
		long sz=size();
		T* buf = new T[sz];
		fread(buf,sz,1,fp);
		return buf;
	}

	template<typename T>
	long load(std::vector<T> &buf){
		long sz=size();
		buf.resize((sz+sizeof(T)-1)/sizeof(T));
		fread(&buf[0],sz,1,fp);
		return sz;
	}
	template<typename T>
	long load(std::vector<T> &buf,long len){
		buf.resize(len);
		fread(&buf[0],sizeof(T),len,fp);
		return len;
	}

	long load(void *buf,long len){
		fread(buf,len,1,fp);
		return len;
	}

	long save(void*buf,int sz){
		return fwrite(buf,sz,1,fp);
	}

	template<typename T>
	long save(const std::vector<T> &buf){
		return fwrite(&buf[0],sizeof(T),buf.size(),fp);
	}

	long size(){
		if (!fp) return -1;
		long sz;
		long t=ftell(fp);
		fseek(fp,0,SEEK_END);
		sz=ftell(fp);
		fseek(fp,t,SEEK_SET); // –ß‚µ‚Ä‚¨‚­
		return sz;
	}
	bool exist(){return size()>=0;}
	bool error(){return fp==NULL;}
	void close(){if (fp) fclose(fp);}
	static int rename(const std::string &p1,const std::string &p2) {
		return ::rename(p1.c_str(),p2.c_str());
	}
};

/*
class File{
public:
	std::string path;
	File(const std::string &fname){path=fname;}
	bool exist(){return size()>=0;}
	long size() {
		HANDLE hPF= CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (hPF == INVALID_HANDLE_VALUE) return -1;
		int size=(int)GetFileSize(hPF, NULL);
		CloseHandle(hPF);
		return size;
	}
	long load(char* buf,int fsize)
	{
		HANDLE hPF= CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if(hPF == INVALID_HANDLE_VALUE)return 0;
		ULONG size;
		ReadFile(hPF, buf, fsize, &size, NULL);
		CloseHandle(hPF);
		return size;
	}
};
*/

class Directory{
	DIR *dir;
public:
	Directory(const std::string &path){open(path);}
	~Directory(){close();}
	int open(const std::string &path){
		dir = opendir(path.c_str());
		return dir!=NULL;
	}
	void close(){
		closedir(dir);
		dir = NULL;
	}
	std::string operator *(){
		dirent *d=readdir(dir);
		if (!d) return "";
		std::string s=d->d_name;
		return s;
	}
};

#endif

