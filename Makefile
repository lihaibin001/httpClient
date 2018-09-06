CC := arm-none-linux-gnueabi-gcc
TARGET = httpClient
obj := httpClient.c kprotocal.c ./cJSON/cJSON.c
LIB = -L/usr/libcurl/lib
INCLUDE = -I/usr/libcurl/include -I./cJSON
LIBS = -lcurl
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c
all:$(obj)
	$(CC) -o $(TARGET) $(obj) $(LIB) $(INCLUDE) $(LIBS)
	# %.o:%.c
.PHONY:clean
clean:
	rm $(TARGET) *.o


