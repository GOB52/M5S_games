/*!
  Breakout
  @brief Simple breakout game
 */

#if __has_include (<M5Unified.h>)
# pragma message "[Breakout2] Using M5Unified"
# if BO2_ENABLE_DISPLAY_MODULE
#   pragma message "[Breakout2] Enable Display module"
#   include <M5ModuleDisplay.h>
# endif
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

#include <esp_system.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include "app.hpp"
#include "sound.hpp"
#include <gob_m5s_sd.hpp>

#ifdef M5UNIFIED_VERSION
auto& display = M5.Display;
#else
static LGFX lcd;
auto& display = lcd;
#endif

# ifndef TFCARD_CS_PIN
#   define TFCARD_CS_PIN (4)
# endif

void setup()
{
    auto mem0 = esp_get_free_heap_size();

    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_BTDM);
    auto mem1 = esp_get_free_heap_size();

#ifdef M5UNIFIED_VERSION
    auto cfg = M5.config();
# if defined(__M5GFX_M5MODULEDISPLAY__)
    cfg.module_display.logical_width = 320;
    cfg.module_display.logical_height = 240;
# endif
    cfg.external_speaker.module_display = true;
    M5.begin(cfg);
    M5.setPrimaryDisplayType(m5::board_t::board_M5ModuleDisplay);
#else    
    M5.begin(false /* LCD */, false /* SD */ , true /* Serial */);
    Wire.begin();
#endif
    while(!Serial){ delay(10); }
    
    SdFat& sd = goblib::m5s::SD::instance().sd(); // SdFat のインスタンス
    while(!sd.begin((unsigned)TFCARD_CS_PIN, SD_SCK_MHZ(25))) { delay(10); } // 開始
    auto mem2 = esp_get_free_heap_size();

    SoundSystem::instance().setup();

    Breakout::instance().setup(&display);

    auto mem3 = esp_get_free_heap_size();

    printf("Heap available : %u : %u : %u : %u\n", mem0, mem1, mem2, mem3);
}
    
void loop()
{
    Breakout::instance().pump();
}
