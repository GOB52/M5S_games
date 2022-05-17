/*!
  Breakout
  @brief Simple breakout game
 */
#include <M5Stack.h>
#ifdef min
#undef min
#endif
#include <LovyanGFX.hpp>
#include "app.hpp"

static LGFX lcd;

void setup()
{
    M5.begin(false /* LCD */, false /* SD */ , true /* Serial */);
    Wire.begin();
    while(!Serial){ delay(10); }

    Breakout::instance().setup(&lcd);
}
    
void loop()
{
    Breakout::instance().pump();
}
