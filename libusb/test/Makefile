CC := arm-none-linux-gnueabi-gcc
TARGET = usb_test 
obj := test3.c 
LIB = -L/root/tool/libusb/lib
INCLUDE = -I/root/tool/libusb/include/
LIBS = -lusb-1.0 -lrt
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c
all:$(obj)
	$(CC) -o $(TARGET) $(obj) $(LIB) $(INCLUDE) $(LIBS)
	# %.o:%.c
.PHONY:clean
clean:
	rm $(TARGET) *.o


