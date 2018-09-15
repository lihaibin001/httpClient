CC := arm-none-linux-gnueabi-gcc
TARGET = router
SRCS := httpClient.c kprotocal.c ./cJSON/cJSON.c usbCom.c router.c List.c
OBJS := $(SRCS:%.c=%.o)
# LIB = -L/usr/libcurl/lib -L./libusb/lib
LIB = -L./libcurl/lib -L./libusb/lib
# INCLUDE = -I/usr/libcurl/include -I./cJSON -I/usr/libusb/include/
INCLUDE = -I./libcurl/include -I./cJSON -I./libusb/include/
LIBS = -lcurl -lusb-1.0 -lrt
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c
all:$(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(LIB) $(INCLUDE) $(LIBS)
.PHONY:clean
clean:
	rm $(TARGET) 


