/*!
  Breakout
  @brief Simple breakout game
*/
#ifndef BREAKOUT_APP_HPP
#define BREAKOUT_APP_HPP

#include "debug.hpp"
#include "typedef.hpp"
#include "breakout.hpp"
#include <gob_macro.hpp>
#include <gob_app.hpp>
#include <gob_singleton.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <gob_m5s_faces.hpp>
#include <vector>

using AppClock = std::chrono::steady_clock;
#define MAX_FPS (30)

class Breakout : public goblib::App<AppClock, MAX_FPS, MAX_FPS>, goblib::Singleton<Breakout>
{
  public:
    // Resolve ambiguities.
    using PointerType = std::unique_ptr<Breakout>;
    using Singleton<Breakout>::instance;
    using Singleton<Breakout>::create;

#if __has_include (<M5Unified.h>)
    void setup(M5GFX* lcd);
#else
    void setup(LGFX* lcd);
#endif

    virtual void fixedUpdate() override;
    virtual void update(float delta) override;
    virtual void render() override;

    void addScore(std::uint32_t pts) { _score += pts; }
    
  protected:
    friend class goblib::Singleton<Breakout>;
    Breakout();

    virtual void sleep_until(const std::chrono::time_point<AppClock, UpdateDuration>& abs_time) override
    {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(abs_time - AppClock::now()).count();
        auto ms = us > 0 ? us / 1000 : 0;
        delay(ms);
        while(AppClock::now() < abs_time){ taskYIELD(); }
    }

    void phaseStart();
    void phaseGame();
    void phaseClear();
    void phaseMiss();

    void renderStart(LGFX_Sprite* s, std::int_fast16_t yoffset);
    void renderGame(LGFX_Sprite* s, std::int_fast16_t yoffset);
    void renderClear(LGFX_Sprite* s, std::int_fast16_t yoffset);
    void renderMiss(LGFX_Sprite* s, std::int_fast16_t yoffset);

    void rewindBall();
    
  private:
#if __has_include (<M5Unified.h>)
    M5GFX* _lcd;
#else
    LGFX* _lcd;;
#endif
    LGFX_Sprite _sprites[2];
    goblib::m5s::FaceGB _input;
    std::uint32_t _lcd_width, _lcd_height;
    bool _flip;

    //
    enum Phase { Start, Game, Clear, Miss };
    Phase _phase;

    std::vector<Ball> _balls;
    Paddle _paddle;
    Bricks _bricks;

    std::uint32_t _score;
    std::int8_t _remain;
    std::int8_t _stage;
    std::int32_t _cnt;

    constexpr static std::int8_t REMAIN = 3;
};

#endif
