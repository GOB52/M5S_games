/*!
  m5s_app
  @brief Application class sample
  @sa https://github.com/lovyan03/LovyanGFX/tree/master/examples/Sprite/MovingIcons
*/
#ifndef M5S_APP_HPP
#define M5S_APP_HPP

#include <gob_macro.hpp>
#include <gob_app.hpp>
#include <gob_singleton.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <array>

using AppClock = std::chrono::steady_clock;

// Maximum frane per second (best effort)
#define MAX_FPS (30)

class M5S_App : public goblib::App<AppClock, MAX_FPS, MAX_FPS>, goblib::Singleton<M5S_App>
{
  public:
    // Resolve ambiguities.
    using PointerType = std::unique_ptr<M5S_App>;
    using Singleton<M5S_App>::instance;
    using Singleton<M5S_App>::create;

#if __has_include (<M5Unified.h>)
    void setup(M5GFX* lcd);
#else
    void setup(LGFX* lcd);
#endif
    
    virtual void fixedUpdate() override;
    virtual void update(float delta) override;
    virtual void render() override;

  protected:
    friend class goblib::Singleton<M5S_App>;
    M5S_App();

    virtual void sleep_until(const std::chrono::time_point<AppClock, UpdateDuration>& abs_time) override
    {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(abs_time - AppClock::now()).count();
        auto ms = us > 0 ? us / 1000 : 0;
        delay(ms);
        while(AppClock::now() < abs_time){ taskYIELD(); }
    }

  private:
#if __has_include (<M5Unified.h>)
    M5GFX* _lcd;
#else
    LGFX* _lcd;;
#endif
    std::uint32_t _lcd_width, _lcd_height;
    LGFX_Sprite _sprites[2];
    bool _flip;
};

#endif
