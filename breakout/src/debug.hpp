/*!
  Breakout
  @brief Simple breakout game
*/
#ifndef BREAKOUT_DEBUG_HPP
#define BREAKOUT_DEBUG_HPP

//#define INCLUDE_HIT_BOTTOM_OF_FIELD
//#define DEBUG_RENDER

#if 0
#include <cstdio>
#define PRINTF_S(str) do { printf("%s", (str)); } while(0)
#define PRINTF(fmt, ...) do { printf((fmt), __VA_ARGS__); }while(0)

#else

#define PRINTF_S(str) /* nop */
#define PRINTF(fmt, ...) /* nop */

#endif

#endif
