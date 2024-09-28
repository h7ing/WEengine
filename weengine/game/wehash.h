/**
 * simplified hash from Linux hash
 */

#ifndef WEHASH_H
#define WEHASH_H

#include <stdint.h>

#if defined(__LP64__) || defined(_WIN64)
# define WE_WORDSIZE 64
#elif defined(__SIZEOF_POINTER__)
# if __SIZEOF_POINTER__ == 4
#  define WE_WORDSIZE 32
# elif __SIZEOF_POINTER__ == 8
#  define WE_WORDSIZE 64
# endif
#else
# define WE_WORDSIZE 32
#endif

#define BITS_PER_LONG WE_WORDSIZE

#if BITS_PER_LONG == 32
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_32
#define hash_long(val, bits) hash_32(val, bits)
#elif BITS_PER_LONG == 64
#define hash_long(val, bits) hash_64(val, bits)
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_64
#else
#error Wordsize not 32 or 64
#endif

#define GOLDEN_RATIO_32 0x61C88647
#define GOLDEN_RATIO_64 0x61C8864680B583EBull

/*
 * The _generic versions exist only so lib/test_hash.c can compare
 * the arch-optimized versions with the generic.
 *
 * Note that if you change these, any <asm/hash.h> that aren't updated
 * to match need to have their HAVE_ARCH_* define values updated so the
 * self-test will not false-positive.
 */
#ifndef HAVE_ARCH__HASH_32
#define __hash_32 __hash_32_generic
#endif
static inline uint32_t __hash_32_generic(uint32_t val)
{
	return val * GOLDEN_RATIO_32;
}

static inline uint32_t hash_32(uint32_t val, unsigned int bits)
{
	/* High bits are more random, so use them. */
	return __hash_32(val) >> (32 - bits);
}

#ifndef HAVE_ARCH_HASH_64
#define hash_64 hash_64_generic
#endif
static inline uint32_t hash_64_generic(uint64_t val, unsigned int bits)
{
#if BITS_PER_LONG == 64
	/* 64x64-bit multiply is efficient on all 64-bit processors */
	return val * GOLDEN_RATIO_64 >> (64 - bits);
#else
	/* Hash 64 bits using only 32x32-bit multiply. */
	return hash_32((uint32_t)val ^ __hash_32(val >> 32), bits);
#endif
}

static inline uint32_t hash_ptr(const void *ptr, unsigned int bits)
{
	return hash_long((unsigned long)ptr, bits);
}

/* This really should be called fold32_ptr; it does no hashing to speak of. */
static inline uint32_t hash32_ptr(const void *ptr)
{
	unsigned long val = (unsigned long)ptr;

#if BITS_PER_LONG == 64
	val ^= (val >> 32);
#endif
	return (uint32_t)val;
}

#endif