#
# This is a simple Makefile to test some of the RAID-6 code
# from userspace.
#

CC	 = gcc
OPTFLAGS = -O2			# Adjust as desired
CFLAGS	 = -I.. -I ../../../include -g $(OPTFLAGS)
LD	 = ld
AWK	 = awk -f
AR	 = ar
RANLIB	 = ranlib

ifeq ($(ARCH),arm)
        CFLAGS += -I../../../arch/arm/include -mfpu=neon \
          -DCONFIG_KERNEL_MODE_NEON=1
      OBJS += neon.o neon1.o neon2.o neon4.o neon8.o
endif

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

%.c: ../%.c
	cp -f $< $@

%.uc: ../%.uc
	cp -f $< $@

all:	raid6.a raid6test

raid6.a: int1.o int2.o int4.o int8.o int16.o int32.o mmx.o sse1.o sse2.o \
	 altivec1.o altivec2.o altivec4.o altivec8.o recov.o algos.o \
	 tables.o
	 rm -f $@
	 $(AR) cq $@ $^
	 $(RANLIB) $@

raid6test: test.c raid6.a
	$(CC) $(CFLAGS) -o raid6test $^

neon1.c: neon.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=1 < neon.uc > $@

neon2.c: neon.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=2 < neon.uc > $@

neon4.c: neon.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=4 < neon.uc > $@

neon8.c: neon.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=8 < neon.uc > $@

altivec1.c: altivec.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=1 < altivec.uc > $@

altivec2.c: altivec.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=2 < altivec.uc > $@

altivec4.c: altivec.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=4 < altivec.uc > $@

altivec8.c: altivec.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=8 < altivec.uc > $@

int1.c: int.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=1 < int.uc > $@

int2.c: int.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=2 < int.uc > $@

int4.c: int.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=4 < int.uc > $@

int8.c: int.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=8 < int.uc > $@

int16.c: int.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=16 < int.uc > $@

int32.c: int.uc ../unroll.awk
	$(AWK) ../unroll.awk -vN=32 < int.uc > $@

tables.c: mktables
	./mktables > tables.c

clean:
	rm -f *.o *.a mktables mktables.c *.uc int*.c altivec*.c neon*.c tables.c raid6test

spotless: clean
	rm -f *~
