
#pragma once

/*
 * Our subset of sys/types.h
 */

typedef unsigned int size_t;

typedef unsigned char 		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int 		uint32_t;
typedef unsigned long long 	uint64_t;

typedef char 		int8_t;
typedef short		int16_t;
typedef int 		int32_t;
typedef long long 	int64_t;

#define false 0
#define true 1
typedef uint8_t bool;

typedef unsigned int pid_t;