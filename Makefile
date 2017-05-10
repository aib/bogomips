CFLAGS = -Wall -O2
LDLIBS = -lpthread

all: bogomips

clean:
	$(RM) *.o bogomips
