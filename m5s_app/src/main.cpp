/*!
  m5s_app
  @brief Application class sample
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
    while(!Serial){ delay(10); }

    m5s_app::instance().setup(&lcd);
}
    
void loop()
{
    m5s_app::instance().pump();
}
