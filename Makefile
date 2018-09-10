CC := arm-none-linux-gnueabi-gcc
TARGET = router
SRCS := httpClient.c kprotocal.c ./cJSON/cJSON.c usbCom.c router.c List.c
OBJS := $(SRCS:%.c=%.o)
LIB = -L/usr/libcurl/lib
INCLUDE = -I/usr/libcurl/include -I./cJSON
LIBS = -lcurl
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c
all:$(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(OBJS) $(LIB) $(INCLUDE) $(LIBS)
.PHONY:clean
clean:
	rm $(TARGET) *.o


