include ../../../Rule.mk
LIB=libpoe.so
CFLAGS= -c -Os -s -DPNUM=$(PNUM) -DGNUM=$(GNUM) -D$(SYSNAME)

CFLAGS += -DBCM_DEBUG=$(BCM_DEBUG)
OBJS=PoE.o poe_tools.o SPP_Events.o SPP_Serial.o SPP_Util.o
poeLibDir =  $(shell pwd)/poe
INCLUDES = -I.

all: $(LIB)

$(LIB): $(OBJS)
	$(CC) -Os -s -shared -Wl,-soname,$(LIB) -lrt -lpthread -lm -o $(LIB) $(OBJS)

%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

install:
	$(STRIP) $(LIB)
	cp -a $(LIB) ../../../target/lib 
    
clean:
	@echo Cleaning up
	rm -f *.o *.so

