/*!
  Breakout
  @brief Simple breakout game
 */
#include <M5Stack.h>
#ifdef min
#undef min
#endif
#include <LovyanGFX.hpp>
#include <esp_system.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include "app.hpp"
#include "sound.hpp"
#include <gob_m5s_sd.hpp>

static LGFX lcd;

void setup()
{
    auto mem0 = esp_get_free_heap_size();

    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_BTDM);
    auto mem1 = esp_get_free_heap_size();
    
    M5.begin(false /* LCD */, false /* SD */ , true /* Serial */);
    Wire.begin();
    while(!Serial){ delay(10); }

    SdFat& sd = goblib::m5s::SD::instance().sd(); // SdFat のインスタンス
    while(!sd.begin((unsigned)TFCARD_CS_PIN, SD_SCK_MHZ(25))) { delay(10); } // 開始
    auto mem2 = esp_get_free_heap_size();

    SoundSystem::instance().setup();
    Breakout::instance().setup(&lcd);
    auto mem3 = esp_get_free_heap_size();

    printf("Heap available : %u : %u : %u : %u\n", mem0, mem1, mem2, mem3);
}
    
void loop()
{
    Breakout::instance().pump();
}
