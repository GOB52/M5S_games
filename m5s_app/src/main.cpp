/*!
  m5s_app
  @brief Application class sample
 */
#if __has_include (<M5Unified.h>)
# include <SdFat.h>
# include <M5Unified.h>
# include <M5GFX.h>
#else
# pragma message "[Breakout2] Using LovyanGFX"
# include <M5stack.h>
# ifdef min
#   undef min
# endif
# include <LovyanGFX.hpp>
# include <SdFat.h>
#endif
#include "app.hpp"

#ifdef M5UNIFIED_VERSION
auto& display = M5.Display;
#else
static LGFX lcd;
auto& display = lcd;
#endif

void setup()
{
#ifdef M5UNIFIED_VERSION
    auto cfg = M5.config();
    M5.begin(cfg);
#else    
    M5.begin(false /* LCD */, false /* SD */ , true /* Serial */);
    Wire.begin();
#endif
    M5S_App::instance().setup(&display);
}
    
void loop()
{
    M5S_App::instance().pump();
}
