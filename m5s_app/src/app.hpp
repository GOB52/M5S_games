/*!
  m5s_app
  @sa https://github.com/lovyan03/LovyanGFX/tree/master/examples/Sprite/MovingIcons
*/
#ifndef M5S_APP_HPP
#define M5S_APP_HPP

#include <gob_macro.hpp>
#include <gob_app.hpp>
#include <gob_singleton.hpp>
#include <gob_m5s_clock.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <array>

#ifdef LGFX_USE_V1
#include <lgfx/v1_autodetect/LGFX_AutoDetect_ESP32.hpp>
#else
class LGFX;
#endif

using AppClock = goblib::m5s::esp_clock;

// Maximum frane per second (best effort)
#define MAX_FPS (60)

class m5s_app : public goblib::App<AppClock, MAX_FPS, MAX_FPS>, goblib::Singleton<m5s_app>
{
  public:
    // Resolve ambiguities.
    using PointerType = std::unique_ptr<m5s_app>;
    using Singleton<m5s_app>::instance;
    using Singleton<m5s_app>::create;

    void setup(LGFX* lcd);
    
    virtual void fixedUpdate() override;
    virtual void update(float delta) override;
    virtual void render() override;

  protected:
    friend class goblib::Singleton<m5s_app>;
    m5s_app();

    virtual void sleep_until(const std::chrono::time_point<AppClock, UpdateDuration>& abs_time) override
    {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(abs_time - AppClock::now()).count();
        auto ms = us > 0 ? us / 1000 : 0;
        delay(ms);
        while(AppClock::now() < abs_time){ taskYIELD(); }
    }

  private:
    LGFX* _lcd;;
    std::uint32_t _lcd_width, _lcd_height;
    goblib::lgfx::GSprite _sprites[2];
    bool _flip;
};

#endif
