
#include ../Makefile.inc
include ../root/Makefile.inc

OBJECTS_DIR = ./

HEADERS = 
SOURCES = Comm.cpp \
	Console.cpp \
	Date.cpp \
	DateTime.cpp \
	Errors.cpp \
	File_com.cpp \
	File2com.cpp \
	Findfile.cpp \
	Folder.cpp \
	GetStr.cpp \
	Instance.cpp \
	Refs.cpp \
	SysInfo.cpp \
	StringEx.cpp \
	StringEx64.cpp \
	StringLib.cpp \
	md4c.cpp \
	md5.cpp \
	Time.cpp \
	Univhash.cpp \
	int64.cpp \
	perm.cpp \
	unix/File.cpp \
	File2.cpp \
	mthrlib.cpp \
	Lib.cpp \
	MaaEvents.cpp \
	socket2.cpp \
	sk2add.cpp \
	MaaXml.cpp \
	Timer.cpp \
	FileTable.cpp \
	a_trie.cpp \
	resize_lim.cpp \
	curl_wrp.cpp \
	Maps.cpp \
	utils.cpp \
	wwwcgi/cgi_helper.cpp \
	wwwcgi/gd_helper.cpp \
	archiver/haio.cpp \
	archiver/acoder.cpp \
	archiver/hsc.cpp \
	archiver/haffman.cpp

#OBJECTS = Date.o \
#	DateTime.o \
#	String.o \
#	Time.o \
#	Univhash.o \
#	unix/File.o \
#	File_com.o \

OBJECTS = 	Errors.o \
	File2com.o \
	Refs.o \
	SysInfo.o \
	StringEx.o \
	StringEx64.o \
	StringLib.o \
	md4c.o \
	md5.o \
	perm.o \
	File2.o \
	mthrlib.o \
	Lib.o \
	MaaEvents.o \
	socket2.o \
	sk2add.o \
	MaaXml.o \
	Timer.o \
	FileTable.o \
	a_trie.o \
	resize_lim.o \
	curl_wrp.o \
	Maps.o \
	utils.o \
	wwwcgi/cgi_helper.o \
	wwwcgi/gd_helper.o \
	archiver/haio.o \
	archiver/acoder.o \
	archiver/hsc.o \
	archiver/haffman.o \
	tl_64.o

DESTDIR  = 
TARGET   = ToolsLib.a

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(AR) $(TARGET) $(OBJECTS)
	ls -l *.a

#first: all

install:

uninstall:
