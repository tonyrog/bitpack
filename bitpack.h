//
// @author Tony Rogvall <tony@rogvall.se>
// @copyright (C) 2012, Tony Rogvall
//
// BIT PACKING/UNPACKING inline function 
// suitable for code generation
//
//

#ifndef __BITPACK_H__
#define __BITPACK_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
// allow gdb debugging
#define ALWAYS_INLINE 
#else
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

static int inline set_bits_le(uint8_t* ptr, uint32_t value, int i, size_t n)
    ALWAYS_INLINE;
static int inline get_bits_le(uint8_t* ptr, uint32_t* value, int i, size_t n)
    ALWAYS_INLINE;
static int inline copy_bits_le(uint8_t* src,uint32_t soffs,
			       uint8_t* dst,uint32_t doffs,
			       size_t n);
 static int inline seq_bits_le(uint8_t* ptr, uint32_t value, int i, size_t n)
     ALWAYS_INLINE;
 static int inline clr_bits_le(uint8_t* ptr, int i, size_t n)
     ALWAYS_INLINE;
 static int inline clq_bits_le(uint8_t* ptr, int i, size_t n)
     ALWAYS_INLINE;

 static int inline set_bits_be(uint8_t* ptr, uint32_t value, int i, size_t n)
     ALWAYS_INLINE;
 static int inline get_bits_be(uint8_t* ptr, uint32_t* value, int i, size_t n)
     ALWAYS_INLINE;
 static int inline copy_bits_be(uint8_t* src,uint32_t soffs,
				uint8_t* dst,uint32_t doffs,
				size_t n);
 static int inline seq_bits_be(uint8_t* ptr, uint32_t value, int i, size_t n)
     ALWAYS_INLINE;
 static int inline clr_bits_be(uint8_t* ptr, int i, size_t n)
     ALWAYS_INLINE;
 static int inline clq_bits_be(uint8_t* ptr, int i, size_t n)
     ALWAYS_INLINE;

 #define MAKE_MASK(n) ((((uint32_t) 1) << (n))-1)
 #define MASK_BITS(src,dst,mask) (((src) & (mask)) | ((dst) & ~(mask)))
 #define BYTE_OFFSET(ofs) ((uint32_t) (ofs) >> 3)
 #define BIT_OFFSET(ofs)  ((ofs) & 7)

 //
 // write n bits into byte array pointed to by ptr,
 // from bit position i to bit position i+n  (n <= 32) from bits
 //       0        1        2        3        4     
 //   +--------+--------+--------+--------+--------+
 //   |        |        |        |        |        |
 //   +--------+--------+--------+--------+--------+
 // i  01234567 01234567 01234567 01234567 01234567  j
 //                  ^
 // n=11        11111100 00011111  (lmask,rmask)
 //
 static int inline set_bits_le(uint8_t* ptr, uint32_t value, int i, size_t n)
 {
  // |b7..b0|b15...b8|....
 #define L_MASK(x)          (MAKE_MASK(8-(x))<<(x))
 #define R_MASK(x)          MAKE_MASK((x))
 #define S_SHIFT(bits,n)    ((bits) >> (n))
 #define D_SHIFT(bits,n)    ((bits) << (n))
     int j = i+n;        // right position
     int r = j;          // next position when packing
     int k = i >> 3;     // byte position

     i = BIT_OFFSET(i);
     j = BIT_OFFSET(j);

     if ((i+n) < 8) {  // all bit in the same byte
	 uint8_t mask = L_MASK(i) & R_MASK(j);
	 uint32_t src = value<<i;
	 ptr[k] = MASK_BITS(src, ptr[k], mask);
     }
     else {
	 int nk;
	 if (n && i) {
	     uint8_t mask = L_MASK(i);
	     int b = (n < (8-i)) ? n : (8-i);
	     uint32_t src = value<<i;
	     ptr[k] = MASK_BITS(src, ptr[k], mask);
	     k++;
	     n -= b;
	     value >>= b;
	 }
	 switch(nk = (n >> 3)) {
	 case 4: ptr[k+3] = (value >> 24);
	 case 3: ptr[k+2] = (value >> 16);
	 case 2: ptr[k+1] = (value >> 8);
	 case 1: ptr[k] = value;
	     k += nk; nk = (nk << 3); n -= nk; value >>= nk;
	     break;
	 case 0:
	     break;
	 default: return -1;
	 }
	 if (n) {
	     uint8_t mask = R_MASK(j);
	     uint32_t src = value;
	     ptr[k] = MASK_BITS(src, ptr[k], mask);	
	 }
     }
     return r;
 #undef L_MASK
 #undef R_MASK
 #undef S_SHIFT
 #undef D_SHIFT
 }

 //
 // seq_bits is like set_bits but only preserve bits poked
 // at the left side not on the right side!
 // used to sequentially fill bits from left to right
 //
 static int inline seq_bits_le(uint8_t* ptr, uint32_t value, int i, size_t n)
 {
  // |b7..b0|b15...b8|....
 #define L_MASK(x)          (MAKE_MASK(8-(x))<<(x))
 #define R_MASK(x)          MAKE_MASK((x))
 #define S_SHIFT(bits,n)    ((bits) >> (n))
 #define D_SHIFT(bits,n)    ((bits) << (n))
     int j = i+n;        // right position
     int r = j;          // next position when packing
     int k = i >> 3;     // byte position

     i = BIT_OFFSET(i);
     j = BIT_OFFSET(j);

     if ((i+n) < 8) {  // all bit in the same byte
	 uint8_t mask = L_MASK(i);
	 uint32_t src = value<<i;
	 ptr[k] = MASK_BITS(src, ptr[k], mask);
     }
     else {
	 int nk;
	 if (n && i) {
	     uint8_t mask = L_MASK(i);
	     int b = (n < (8-i)) ? n : (8-i);
	     uint32_t src = value<<i;
	     ptr[k] = MASK_BITS(src, ptr[k], mask);
	     k++;
	     n -= b;
	     value >>= b;
	 }
	 switch(nk = (n >> 3)) {
	 case 4: ptr[k+3] = (value >> 24);
	 case 3: ptr[k+2] = (value >> 16);
	 case 2: ptr[k+1] = (value >> 8);
	 case 1: ptr[k] = value;
	     k += nk; nk = (nk << 3); n -= nk; value >>= nk;
	     break;
	 case 0:
	     break;
	 default: return -1;
	 }
	 if (n) {
	     ptr[k] = value;
	 }
     }
     return r;
 #undef L_MASK
 #undef R_MASK
 #undef S_SHIFT
 #undef D_SHIFT
 }

 //
 // copy n bits from src:soffs to dst:doffs using little endian 
 // fill order
 //

 static int inline copy_bits_le(
     uint8_t* src,    // Base pointer to source.
     uint32_t soffs,  // Bit offset relative to src.
     uint8_t* dst,    // Base pointer to destination.
     uint32_t doffs,  // Bit offset relative to dst.
     size_t n)	     // Number of bits to copy.
 {
  // |b7..b0|b15...b8|....
 #define L_MASK(x)          (MAKE_MASK(8-(x))<<(x))
 #define R_MASK(x)          MAKE_MASK((x))
 #define S_SHIFT(bits,n)    ((bits) >> (n))
 #define D_SHIFT(bits,n)    ((bits) << (n))
     uint32_t lmask;
     uint32_t rmask;
     uint32_t count;
     uint32_t deoffs;
     int r = n;


     if (n == 0)
	 return r;
     src   += BYTE_OFFSET(soffs);
     dst   += BYTE_OFFSET(doffs);
     soffs = BIT_OFFSET(soffs);
     doffs = BIT_OFFSET(doffs);

     // Byte copy loop
     if (!soffs && !doffs && !(n & 0x7)) {
	 while(n) {
	     *dst++ = *src++;
	     n -= 8;
	 }
	 return r;
     }
     deoffs = BIT_OFFSET(doffs+n);  // destination end bit-offset
     // left and right destination bit masks
     lmask = L_MASK(doffs);
     rmask = R_MASK(deoffs);

     if ((doffs+n) < 8) {	// All bits are in the same byte
	 lmask = (lmask & rmask) ? (lmask & rmask) : (lmask | rmask);

	 if (soffs == doffs) {
	     *dst = MASK_BITS(*src,*dst,lmask);
	 } else if (soffs > doffs) {
	     uint32_t bits = S_SHIFT(*src, soffs-doffs);
	     if (soffs+n > 8) {
		 src += 1;
		 bits |= D_SHIFT(*src,(8-(soffs-doffs)));
	     }
	     *dst = MASK_BITS(bits,*dst,lmask);
	 } else {
	     *dst = MASK_BITS(D_SHIFT(*src, doffs-soffs),*dst,lmask);
	 }
	 return r;
     }

     // At this point, we know that the bits are in 2 or more bytes.

     count = ((lmask) ? (n - (8 - doffs)) : n) >> 3;

     if (soffs == doffs) {
	 // The bits are aligned in the same way. We can just copy the bytes

	 if (lmask) {
	     *dst = MASK_BITS(*src, *dst, lmask);
	     dst++;
	     src++;
	 }

	 while (count--)
	     *dst++ = *src++;

	 if (rmask) {
	     *dst = MASK_BITS(*src,*dst,rmask);
	 }
     } else {
	 uint32_t bits;
	 uint32_t bits1;
	 uint32_t rshift;
	 uint32_t lshift;

	 // The tricky case. The bits must be shifted into position.
	 if (soffs > doffs) {
	     lshift = (soffs - doffs);
	     rshift = 8 - lshift;
	     bits = *src;
	     if (soffs + n > 8) {
		 src++;
	     }
	 } else {
	     rshift = (doffs - soffs);
	     lshift = 8 - rshift;
	     bits = 0;
	 }

	 if (lmask) {
	     bits1 = S_SHIFT(bits, lshift);
	     bits = *src++;
	     bits1 |= D_SHIFT(bits, rshift);
	     *dst = MASK_BITS(bits1,*dst,lmask);
	     dst++;
	 }

	 while (count--) {
	     bits1 = S_SHIFT(bits,lshift);
	     bits = *src++;
	     *dst++ = bits1 | D_SHIFT(bits,rshift);
	 }

	 if (rmask) {
	     bits1 = S_SHIFT(bits, lshift);
	     if (S_SHIFT(rmask,rshift) & 0xff) {
		 bits = *src;
		 bits1 |= D_SHIFT(bits,rshift);
	     }
	     *dst = MASK_BITS(bits1,*dst,rmask);
	 }
     }
     return r;
 #undef L_MASK
 #undef R_MASK
 #undef S_SHIFT
 #undef D_SHIFT
 }

 // read bits 
 static int inline get_bits_le(uint8_t* ptr, uint32_t* value, int i, size_t n)
 {
  // |b7..b0|b15...b8|....
 #define L_MASK(x)          (MAKE_MASK(8-(x))<<(x))
 #define R_MASK(x)          MAKE_MASK((x))
 #define S_SHIFT(bits,n)    ((bits) >> (n))
 #define D_SHIFT(bits,n)    ((bits) << (n))
     int j = i+n;        // right position
     int r = j;          // next position when packing
     int k = i >> 3;     // byte position

     i = BIT_OFFSET(i);
     j = BIT_OFFSET(j);

     if ((i+n) < 8) {  // all bit in the same byte
	 uint8_t mask = L_MASK(i) & R_MASK(j);
	 *value = (ptr[k] & mask)>>i;
     }
     else {
	 int s=0;
	 int nk;
	 uint32_t v = 0;

	 if (n && i) {
	     uint8_t mask = L_MASK(i);
	     s = (n < (8-i)) ? n : (8-i);
	     v = (ptr[k] & mask) >> i;
	     k++;
	     n -= s;
	 }
	 switch(nk = (n >> 3)) {
	 case 4: v |= (ptr[k+3] << (s+24));
	 case 3: v |= (ptr[k+2] << (s+16));
	 case 2: v |= (ptr[k+1] << (s+8));
	 case 1: v |= (ptr[k] << (s));
	     k += nk; s += (nk<<3); n -= (nk<<3);
	     break;
	 case 0:
	     break;
	 default: return -1;
	 }
	 if (n) {
	     uint8_t mask = R_MASK(j);
	     v |= ((ptr[k] & mask) << s);
	 }
	 *value = v;
     }
     return r;
 #undef L_MASK
 #undef R_MASK
 #undef S_SHIFT
 #undef D_SHIFT
 }

 static int inline clr_bits_le(uint8_t* ptr, int i, size_t n)
 {
     return set_bits_le(ptr, 0, i, n);
 }


 static int inline clq_bits_le(uint8_t* ptr, int i, size_t n)
 {
     return seq_bits_le(ptr, 0, i, n);
 }

 //
 // write n bits into byte array pointed to by ptr,
 // from bit position i to bit position i+n  (n <= 32) from bits
 //       0        1        2        3        4     
 //   +--------+--------+--------+--------+--------+
 //   |        |        |        |        |        |
 //   +--------+--------+--------+--------+--------+
 // i  01234567 01234567 01234567 01234567 01234567  j
 //               ^
 // n=11        00111111 11111000  (lmask,rmask)
 //
 static int inline set_bits_be(uint8_t* ptr, uint32_t value, int i, size_t n)
 {
 #define L_MASK(x)          MAKE_MASK(8-(x))
 #define R_MASK(x)          (MAKE_MASK((x))<<(8-(x)))
 #define S_SHIFT(bits,n)    ((bits) << (n))
 #define D_SHIFT(bits,n)    ((bits) >> (n))
     int j = i+n;        // right position
     int r = j;          // next position when packing
     int k = i >> 3;     // byte position

     i = BIT_OFFSET(i);
     j = BIT_OFFSET(j);

     if ((i+n) < 8) {  // all bit in the same byte
	 uint8_t mask = L_MASK(i) & R_MASK(j);
	 uint32_t src = value << (8-j);
	 ptr[k] = MASK_BITS(src, ptr[k], mask);
     }
     else {
	 int nk;
	 if (n && i) {
	     uint8_t  mask = L_MASK(i);
	     int s = (n < (8-i)) ? 0 : (8-i);
	     uint32_t src = value >> (n-s);
	     ptr[k] = MASK_BITS(src, ptr[k], mask);
	     k++;
	     n -= s;
	 }
	 switch(nk = (n >> 3)) {
	 case 4: ptr[k+3] = (value>>(n-32));
	 case 3: ptr[k+2] = (value>>(n-24));
	 case 2: ptr[k+1] = (value>>(n-16));
	 case 1: ptr[k] = (value>>(n-8));
	     k += nk; n -= (nk << 3);
	     break;
	 case 0: break;
	 default: return -1;
	 }
	 if (n) {
	     uint8_t mask = R_MASK(j);
	     uint32_t src = value << (8-j);
	     ptr[k] = MASK_BITS(src, ptr[k], mask);	
	 }
     }
     return r;
 #undef L_MASK
 #undef R_MASK
 #undef S_SHIFT
 #undef D_SHIFT
 }

 static int inline seq_bits_be(uint8_t* ptr, uint32_t value, int i, size_t n)
 {
 #define L_MASK(x)          MAKE_MASK(8-(x))
 #define R_MASK(x)          (MAKE_MASK((x))<<(8-(x)))
 #define S_SHIFT(bits,n)    ((bits) << (n))
 #define D_SHIFT(bits,n)    ((bits) >> (n))
     int j = i+n;        // right position
     int r = j;          // next position when packing
     int k = i >> 3;     // byte position

     i = BIT_OFFSET(i);
     j = BIT_OFFSET(j);

     if ((i+n) < 8) {  // all bit in the same byte
	 uint8_t mask = L_MASK(i);
	 uint32_t src = value << 8-j;
	 ptr[k] = MASK_BITS(src, ptr[k], mask);
     }
     else {
	 int nk;
	 if (n && i) {
	     uint8_t  mask = L_MASK(i);
	     int l = (n < (8-i)) ? 0 : (8-i);
	     uint32_t src = value >> (n-l);
	     ptr[k] = MASK_BITS(src, ptr[k], mask);
	     k++;
	     n -= l;
	 }
	 switch(nk = (n >> 3)) {
	 case 4: ptr[k+3] = (value>>(n-32));
	 case 3: ptr[k+2] = (value>>(n-24));
	 case 2: ptr[k+1] = (value>>(n-16));
	 case 1: ptr[k] = (value>>(n-8));
	     k += nk; n -= (nk << 3);
	     break;
	 case 0: break;
	 default: return -1;
	 }
	 if (n) {
	     ptr[k] = value << (8-j);
	 }
     }
     return r;
 #undef L_MASK
 #undef R_MASK
 #undef S_SHIFT
 #undef D_SHIFT
 }

 //
 // copy n bits from src:soffs to dst:doffs using big endian 
 // fill order
 //

 static int inline copy_bits_be(
     uint8_t* src,    // Base pointer to source.
     uint32_t soffs,  // Bit offset relative to src.
     uint8_t* dst,    // Base pointer to destination.
     uint32_t doffs,  // Bit offset relative to dst.
     size_t n)	    // Number of bits to copy.
 {
 #define L_MASK(x)          MAKE_MASK(8-(x))
 #define R_MASK(x)          (MAKE_MASK((x))<<(8-(x)))
 #define S_SHIFT(bits,n)    ((bits) << (n))
 #define D_SHIFT(bits,n)    ((bits) >> (n))
     uint32_t lmask;
     uint32_t rmask;
     uint32_t count;
     uint32_t deoffs;
     int r = n;

     if (n == 0)
	 return r;
     src   += BYTE_OFFSET(soffs);
     dst   += BYTE_OFFSET(doffs);
     soffs = BIT_OFFSET(soffs);
     doffs = BIT_OFFSET(doffs);

     // Byte copy loop
     if (!soffs && !doffs && !(n & 0x7)) {
	 while(n) {
	     *dst++ = *src++;
	     n -= 8;
	 }
	 return r;
     }
     deoffs = BIT_OFFSET(doffs+n);  // destination end bit-offset
     // left and right destination bit masks
     lmask = L_MASK(doffs);
     rmask = R_MASK(deoffs);

     if ((doffs+n) < 8) {	// All bits are in the same byte
	 lmask = (lmask & rmask) ? (lmask & rmask) : (lmask | rmask);

	 if (soffs == doffs) {
	     *dst = MASK_BITS(*src,*dst,lmask);
	 } else if (soffs > doffs) {
	     uint32_t bits = S_SHIFT(*src, soffs-doffs);
	     if (soffs+n > 8) {
		 src += 1;
		 bits |= D_SHIFT(*src,(8-(soffs-doffs)));
	     }
	     *dst = MASK_BITS(bits,*dst,lmask);
	 } else {
	     *dst = MASK_BITS(D_SHIFT(*src, doffs-soffs),*dst,lmask);
	 }
	 return r;
     }

     // At this point, we know that the bits are in 2 or more bytes.

     count = ((lmask) ? (n - (8 - doffs)) : n) >> 3;

     if (soffs == doffs) {
	 // The bits are aligned in the same way. We can just copy the bytes

	 if (lmask) {
	     *dst = MASK_BITS(*src, *dst, lmask);
	     dst++;
	     src++;
	 }

	 while (count--)
	     *dst++ = *src++;

	 if (rmask) {
	     *dst = MASK_BITS(*src,*dst,rmask);
	 }
     } else {
	 uint32_t bits;
	 uint32_t bits1;
	 uint32_t rshift;
	 uint32_t lshift;

	 // The tricky case. The bits must be shifted into position.
	 if (soffs > doffs) {
	     lshift = (soffs - doffs);
	     rshift = 8 - lshift;
	     bits = *src;
	     if (soffs + n > 8) {
		 src++;
	     }
	 } else {
	     rshift = (doffs - soffs);
	     lshift = 8 - rshift;
	     bits = 0;
	 }

	 if (lmask) {
	     bits1 = S_SHIFT(bits, lshift);
	     bits = *src++;
	     bits1 |= D_SHIFT(bits, rshift);
	     *dst = MASK_BITS(bits1,*dst,lmask);
	     dst++;
	 }

	 while (count--) {
	     bits1 = S_SHIFT(bits,lshift);
	     bits = *src++;
	     *dst++ = bits1 | D_SHIFT(bits,rshift);
	 }

	 if (rmask) {
	     bits1 = S_SHIFT(bits, lshift);
	     if (S_SHIFT(rmask,rshift) & 0xff) {
		 bits = *src;
		 bits1 |= D_SHIFT(bits,rshift);
	     }
	     *dst = MASK_BITS(bits1,*dst,rmask);
	 }
     }
    return r;
#undef L_MASK
#undef R_MASK
#undef S_SHIFT
#undef D_SHIFT
}

static int inline get_bits_be(uint8_t* ptr, uint32_t* value, int i, size_t n)
{
#define L_MASK(x)          MAKE_MASK(8-(x))
#define R_MASK(x)          (MAKE_MASK((x))<<(8-(x)))
#define S_SHIFT(bits,n)    ((bits) << (n))
#define D_SHIFT(bits,n)    ((bits) >> (n))
    int j = i+n;        // right position
    int r = j;          // next position when packing
    int k = i >> 3;     // byte position

    i = BIT_OFFSET(i);
    j = BIT_OFFSET(j);

    if ((i+n) < 8) {  // all bit in the same byte
	uint8_t mask = L_MASK(i) & R_MASK(j);
	*value = ((ptr[k] & mask) >> (8-j));
    }
    else {
	int s = 0;
	int nk;
	uint32_t v = 0;

	if (n && i) {
	    uint8_t  mask = L_MASK(i);
	    s = (n < (8-i)) ? 0 : (8-i);
	    v = ((ptr[k] & mask)) << (n-s);
	    k++;
	    n -= s;
	}
	switch(nk = (n >> 3)) {
	case 4: v |= (ptr[k+3] << (n-32));
	case 3: v |= (ptr[k+2] << (n-24));
	case 2: v |= (ptr[k+1] << (n-16));
	case 1: v |= (ptr[k] <<   (n-8));
	    k += nk; n -= (nk << 3);
	    break;
	case 0: break;
	default: return -1;
	}
	if (n) {
	    uint8_t mask = R_MASK(j);
	    v |= ((ptr[k] & mask) >> (8-j));
	}
	*value = v;
    }
    return r;
#undef L_MASK
#undef R_MASK
#undef S_SHIFT
#undef D_SHIFT
}


static int inline clr_bits_be(uint8_t* ptr, int i, size_t n)
{
    return set_bits_be(ptr, 0, i, n);
}

static int inline clq_bits_be(uint8_t* ptr, int i, size_t n)
{
    return seq_bits_be(ptr, 0, i, n);
}

// pack n bytes little endian from i .. i+n-1  n=0,1,2,3,4
static int inline set_bytes_le(uint8_t* ptr, uint32_t value, int i, size_t n)
{
    switch(n) {
    case 4: ptr[i+3] = (value>>24);
    case 3: ptr[i+2] = (value>>16);
    case 2: ptr[i+1] = (value>>8);
    case 1: ptr[i]   = value; break;
    case 0: break;
    default: return -1;
    }
    return i+n;
}

// pack n bytes big endian from i .. i+n-1  n=0,1,2,3,4
static int inline set_bytes_be(uint8_t* ptr, uint32_t value, int i, size_t n)
{
    switch(n) {
    case 4: ptr[i++] = (value>>24);
    case 3: ptr[i++] = (value>>16);
    case 2: ptr[i++] = (value>>8);
    case 1: ptr[i] = value; break;
    case 0: break;
    default: return -1;
    }
    return i;
}

static void inline reverse_bytes(uint8_t* ptr, size_t n)
{
    uint8_t* ptr1 = ptr + n - 1;
    while(ptr < ptr1) {
	uint8_t tmp = *ptr;
	*ptr++ = *ptr1;
	*ptr1-- = tmp;
    }
}

#endif
