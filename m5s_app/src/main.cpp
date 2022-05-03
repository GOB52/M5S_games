
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
    //    Wire.begin();
    while(!Serial){ delay(10); }

    m5s_app::instance().setup(&lcd);
}
    
void loop()
{
    m5s_app::instance().pump();
}
