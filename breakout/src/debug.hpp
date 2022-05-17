
#ifndef BREAKOUT_DEBUG_HPP
#define BREAKOUT_DEBUG_HPP

//#define BEHAVIOR_TEST

//#define ONLY_HIT_FIELD
#define INCLUDE_HIT_BOTTOM_OF_FIELD

#if 0
#include <cstdio>
#define PRINTF_S(str) do { printf("%s", (str)); } while(0)
#define PRINTF(fmt, ...) do { printf((fmt), __VA_ARGS__); }while(0)
#else
#define PRINTF_S(str) /* nop */
#define PRINTF(fmt, ...) /* nop */
#endif



#endif

