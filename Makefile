include ../../Rule.mk
LIB=libbcmutils.so
#CC=$(CROSS)gcc
CFLAGS= -c -Os -s -DPNUM=$(PNUM) -DGNUM=$(GNUM) -D$(SYSNAME)  -DPOE=$(POE)  -DPoE_NUM=$(POENUM) -DPORTPOWER_MAX=$(PORTPOWER_MAX)

#added by xichen 110504 for BCM_DEBUG
CFLAGS += -DBCM_DEBUG=$(BCM_DEBUG)
#added by timwang for utp poe
CFLAGS += -DPOE_SI3482
#AR=$(CROSS)ar
ARFLAGS=rc
OBJS=bcmutils.o  if_info.o gmrp.o qos.o vlan.o mac.o stp.o link.o mirror.o rate.o port.o trunk.o acl.o nvram_conf.o socket_tools.o sk_define.o auth.o str.o i2c.o ipmc.o ipv6.o dhcp.o aaa.o linklist.o l3.o
poeLibDir =  $(shell pwd)/poe

all: $(LIB) poeLib

poeLib:
	$(MAKE) -C $(poeLibDir)

$(LIB): $(OBJS)
	$(CC) -Os -s -shared -Wl,-soname,$(LIB) -o $(LIB) $(OBJS)

%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

install:
	$(STRIP) $(LIB)
	cp -a $(LIB) ../../target/lib 
	$(MAKE) -C $(poeLibDir) install
    
clean:
	@echo Cleaning up
	$(MAKE) -C $(poeLibDir) clean
	rm -f *.o *.a

