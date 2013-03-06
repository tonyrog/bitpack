//
// bit pack fucntions
//
#include <stdio.h>
#include <stdlib.h>

#include "bitpack.h"

void dump_bits(uint8_t* ptr, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++) {
	uint8_t mask = 0x80;
	int j = 8;
	while(j--) {
	    printf("%c", ((*ptr & mask) != 0)+'0');
	    mask >>= 1;
	}
	printf("|");
	ptr++;
    }
    printf("\n");
}

//
// test structures
//
struct s_10_5_17 {
    unsigned l:10;
    unsigned m:5;
    unsigned r:17;
};

struct r_10_5_17 {
    unsigned r:17;
    unsigned m:5;
    unsigned l:10;
};

struct s_6_5_21 {
    unsigned l:6;
    unsigned m:5;
    unsigned r:21;
};

struct r_6_5_21 {
    unsigned r:21;
    unsigned m:5;
    unsigned l:6;
};

struct s_13_5_14 {
    unsigned l:13;
    unsigned m:5;
    unsigned r:14;
};

struct r_13_5_14 {
    unsigned r:14;
    unsigned m:5;
    unsigned l:13;
};

struct s_5_13_14 {
    unsigned l:5;
    unsigned m:13;
    unsigned r:14;
};

struct r_5_13_14 {
    unsigned r:14;
    unsigned m:13;
    unsigned l:5;
};

struct s_2_27_3 {
    unsigned l:2;
    unsigned m:27;
    unsigned r:3;
};

struct r_2_27_3 {
    unsigned r:3;
    unsigned m:27;
    unsigned l:2;
};

struct s_13_3_16 {
    unsigned l:13;
    unsigned m:3;
    unsigned r:16;
};

struct r_13_3_16 {
    unsigned r:16;
    unsigned m:3;
    unsigned l:13;
};

struct s_8_8_16 {
    unsigned l:8;
    unsigned m:8;
    unsigned r:16;
};

struct r_8_8_16 {
    unsigned r:16;
    unsigned m:8;
    unsigned l:8;
};

struct s_8_3_21 {
    unsigned l:8;
    unsigned m:3;
    unsigned r:21;
};

struct r_8_3_21 {
    unsigned r:21;
    unsigned m:3;
    unsigned l:8;
};

typedef union {
    uint32_t v32;
    uint8_t v[4];
    struct s_10_5_17 v10_5_17;
    struct s_6_5_21  v6_5_21;
    struct s_13_5_14 v13_5_14;
    struct s_5_13_14 v5_13_14;
    struct s_2_27_3  v2_27_3;
    struct s_13_3_16 v13_3_16;
    struct s_8_8_16  v8_8_16;
    struct s_8_3_21  v8_3_21;
    // reversed s structure for big/little conversion test
    struct r_10_5_17 w10_5_17;
    struct r_6_5_21  w6_5_21;
    struct r_13_5_14 w13_5_14;
    struct r_5_13_14 w5_13_14;
    struct r_2_27_3  w2_27_3;
    struct r_13_3_16 w13_3_16;
    struct r_8_8_16  w8_8_16;
    struct r_8_3_21  w8_3_21;
} u32_t;


typedef struct profile {
    uint32_t value;
    size_t size; 
} profile_t;

profile_t profile0[] = {  { 3,    3 },
			  { 255,  10 },
			  { 4095, 13 },
			  { 63,   6 } };

profile_t profile1[] = { { 0x7,    3 },
			 { 0x3ff,  10 },
			 { 0x1fff, 13 },
			 { 0x3f,   6 } };

profile_t profile[] = { { 2,  3 },
			{ 3,  10 },
			{ 7,  13 },
			{ 13,  6 } };
void break_here()
{
    // gdb point
}

void check_value(int i, uint32_t ival)
{
    u32_t u;
    uint8_t* base = &u.v[0];

#define CHECK_BITS_LE(pat,ptr,val,ln,mn,rn) do {			\
	uint32_t value = (val) & ((1 << (mn))-1);			\
	uint32_t ivalue;						\
	u.v32 = (pat);							\
	set_bits_le((ptr),value,(ln),(mn));				\
	if ( \
	    (u.v##ln##_##mn##_##rn.l != ((pat) & ((1 << (ln))-1))) ||	\
	    (u.v##ln##_##mn##_##rn.m != value) ||			\
	    (u.v##ln##_##mn##_##rn.r != ((pat) & ((1 << (rn))-1)))) {	\
	    fprintf(stderr, "FAIL: case LE:%d_%d_%d, value=%d\n",(ln),(mn),(rn),(val)); \
	    dump_bits(u.v, sizeof(u));					\
	    break_here();						\
	    u.v32 = (pat);						\
	    set_bits_le((ptr),value,(ln),(mn));				\
	    exit(1);							\
	}								\
	get_bits_le((ptr),&ivalue,(ln),(mn));				\
	if (ivalue != value) { \
	    fprintf(stderr, "FAIL: get LE:%d_%d_%d, value=%d, ivalue=%d\n",(ln),(mn),(rn),value,ivalue); \
	    break_here();						\
	    get_bits_le((ptr),&ivalue,(ln),(mn));			\
	    exit(1);							\
	}								\
    } while(0)

#define CHECK_BITS_BE(pat,ptr,val,ln,mn,rn) do {			\
	uint32_t value = (val) & ((1 << (mn))-1);			\
	uint32_t ivalue;						\
	u.v32 = (pat);							\
	set_bits_be((ptr),value,(ln),(mn));				\
	reverse_bytes((ptr),sizeof(u));					\
	if (								\
	    (u.w##ln##_##mn##_##rn.l != ((pat) & ((1 << (ln))-1))) ||	\
	    (u.w##ln##_##mn##_##rn.m != value) ||			\
	    (u.w##ln##_##mn##_##rn.r != ((pat) & ((1 << (rn))-1)))) {	\
	    fprintf(stderr, "FAIL: case BE:%d_%d_%d, value=%d\n",(ln),(mn),(rn),(val)); \
	    dump_bits(u.v, sizeof(u));					\
	    break_here();						\
	    u.v32 = (pat);						\
	    set_bits_be((ptr),value,(ln),(mn));				\
	    exit(1);							\
	}								\
	reverse_bytes((ptr),sizeof(u));					\
	get_bits_be((ptr),&ivalue,(ln),(mn));				\
	if (ivalue != value) { \
	    fprintf(stderr, "FAIL: get BE:%d_%d_%d, value=%d, ivalue=%d\n",(ln),(mn),(rn),value,ivalue); \
	    break_here();						\
	    get_bits_be((ptr),&ivalue,(ln),(mn));			\
	    exit(1);							\
	}								\
    } while(0)

    switch(i) {
    case 0:
	CHECK_BITS_LE(0x00000000,base,ival,10,5,17);
	CHECK_BITS_LE(0xffffffff,base,ival,10,5,17);

	CHECK_BITS_BE(0x00000000,base,ival,10,5,17);
	CHECK_BITS_BE(0xffffffff,base,ival,10,5,17);
	break;
    case 1: 
	CHECK_BITS_LE(0x00000000,base,ival,6,5,21);
	CHECK_BITS_LE(0xffffffff,base,ival,6,5,21);

	CHECK_BITS_BE(0x00000000,base,ival,6,5,21);
	CHECK_BITS_BE(0xffffffff,base,ival,6,5,21);
	break;
    case 2:
	CHECK_BITS_LE(0x00000000,base,ival,13,5,14);
	CHECK_BITS_LE(0xffffffff,base,ival,13,5,14);

	CHECK_BITS_BE(0x00000000,base,ival,13,5,14);
	CHECK_BITS_BE(0xffffffff,base,ival,13,5,14);
	break;
    case 3:
	CHECK_BITS_LE(0x00000000,base,ival,5,13,14);
	CHECK_BITS_LE(0xffffffff,base,ival,5,13,14);

	CHECK_BITS_BE(0x00000000,base,ival,5,13,14);
	CHECK_BITS_BE(0xffffffff,base,ival,5,13,14);
	break;
    case 4:
	CHECK_BITS_LE(0x00000000,base,ival,2,27,3);
	CHECK_BITS_LE(0xffffffff,base,ival,2,27,3);

	CHECK_BITS_BE(0x00000000,base,ival,2,27,3);
	CHECK_BITS_BE(0xffffffff,base,ival,2,27,3);
	break;
    case 5:
	CHECK_BITS_LE(0x00000000,base,ival,13,3,16);
	CHECK_BITS_LE(0xffffffff,base,ival,13,3,16);

	CHECK_BITS_BE(0x00000000,base,ival,13,3,16);
	CHECK_BITS_BE(0xffffffff,base,ival,13,3,16);
	break;
    case 6:
	CHECK_BITS_LE(0x00000000,base,ival,8,8,16);
	CHECK_BITS_LE(0xffffffff,base,ival,8,8,16);

	CHECK_BITS_BE(0x00000000,base,ival,8,8,16);
	CHECK_BITS_BE(0xffffffff,base,ival,8,8,16);
	break;
    case 7:
	CHECK_BITS_LE(0x00000000,base,ival,8,3,21);
	CHECK_BITS_LE(0xffffffff,base,ival,8,3,21);

	CHECK_BITS_BE(0x00000000,base,ival,8,3,21);
	CHECK_BITS_BE(0xffffffff,base,ival,8,3,21);
	break;
    }
}

void test1()
{
    u32_t u;
    uint32_t value;
    int i,j,k;

    if (sizeof(u) != sizeof(uint32_t)) {
	printf("insanity: sizeof(u) = %zd\n", sizeof(u));
	exit(1);
    }

    // check values in range
    for (value = 7; value > 0; value--) {
	for (i = 0; i < 8; i++) 
	    check_value(i, value);
    }

    // random testing
    srandom(time(0) + getpid());
    for (j = 0; j < 100000; j++) {
	value = random();
	for (i = 0; i < 8; i++)
	    check_value(i, value);
    }
}

main()
{
    test1();
    exit(0);
}
