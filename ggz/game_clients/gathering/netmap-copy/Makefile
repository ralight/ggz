OBJLIST=Netmap.o \
	NetmapMap.o \
	NetmapResource.o \
		NetmapResourceFile.o \
			NetmapResourceFileText.o \
			NetmapResourceFileBinary.o \
		NetmapResourceSocket.o \
	StringExt.o \
	Nvtile.o

TARGET=/usr/local

default: all

%.o: %.cpp
	g++ -I . -c $< -o $@

all: libnetmap.so

libnetmap.so: $(OBJLIST)
	gcc -export-symbols -shared -Wl,-soname,libnetmap.so -o libnetmap.so.1 $(OBJLIST)

clean:
	rm -rf *.o
	rm -f lib*.so*

install:
	install libnetmap.so.1 $(TARGET)/lib
	ln -s $(TARGET)/lib/libnetmap.so.1 $(TARGET)/lib/libnetmap.so
