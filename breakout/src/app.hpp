/*!
  Breakout
  @brief Simple breakout game
*/
#ifndef BREAKOUT_APP_HPP
#define BREAKOUT_APP_HPP

#include "debug.hpp"
#include "typedef.hpp"
#include <gob_macro.hpp>
#include <gob_app.hpp>
#include <gob_singleton.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <gob_m5s_faces.hpp>
#include <array>

#ifdef LGFX_USE_V1
#include <lgfx/v1_autodetect/LGFX_AutoDetect_ESP32.hpp>
#else
class LGFX;
#endif




using AppClock = std::chrono::steady_clock;

// Maximum frane per second (best effort)
#define MAX_FPS (30)

class Ball;


class Breakout : public goblib::App<AppClock, MAX_FPS, MAX_FPS>, goblib::Singleton<Breakout>
{
  public:
    // Resolve ambiguities.
    using PointerType = std::unique_ptr<Breakout>;
    using Singleton<Breakout>::instance;
    using Singleton<Breakout>::create;

    void setup(LGFX* lcd);
    
    virtual void fixedUpdate() override;
    virtual void update(float delta) override;
    virtual void render() override;

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

  private:
    enum BallType { BALL_TYPE_1, BALL_TYPE_2, BALL_TYPE_3 };

#ifdef BEHAVIOR_TEST
    void testBehavior(const BallType type, const std::int32_t i);
    Ball* createBall(const BallType type, const std::int16_t x, const std::int16_t y, const Vec2& v, const float vel);
#endif

  private:
    LGFX* _lcd;;
    goblib::lgfx::GSprite _sprites[2];
    goblib::m5s::FaceGB _input;
    std::uint32_t _lcd_width, _lcd_height;
    bool _flip;

#ifdef BEHAVIOR_TEST
    std::int32_t _cnt;
#endif
    

};

#endif
