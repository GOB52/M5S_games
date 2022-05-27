/*!
  Breakout
  @brief Simple breakout game
*/
#include <M5Stack.h>
#ifdef min
#undef min
#endif
#include <LovyanGFX.hpp>
#include "breakout.hpp"
#include "app.hpp"

namespace
{
int_fast16_t sprite_height;

using StageData = std::vector<std::uint8_t>;
std::vector<StageData> stage =
{
#if 0
    {
        1,0,0,0,0,0,0,0,0,0,0,0,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        9,0,9,0,9,0,9,0,9,0,9,0,9,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,9,0,9,0,9,0,9,0,9,0,9,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        9,9,0,0,9,9,0,0,9,0,9,0,9,
        9,0,0,0,0,9,0,0,9,0,9,0,9,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        9,9,0,0,9,9,0,0,9,9,0,9,9,
    },
#endif
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,3,3,3,
        4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,
    },
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        2,0,0,0,0,0,0,0,0,0,0,0,0,
        2,3,0,0,0,0,0,0,0,0,0,0,0,
        2,3,4,0,0,0,0,0,0,0,0,0,0,
        2,3,4,5,0,0,0,0,0,0,0,0,0,
        2,3,4,5,6,0,0,0,0,0,0,0,0,
        2,3,4,5,6,7,0,0,0,0,0,0,0,
        2,3,4,5,6,7,8,0,0,0,0,0,0,
        2,3,4,5,6,7,8,2,0,0,0,0,0,
        2,3,4,5,6,7,8,2,3,0,0,0,0,
        2,3,4,5,6,7,8,2,3,4,0,0,0,
        2,3,4,5,6,7,8,2,3,4,5,0,0,
        2,3,4,5,6,7,8,2,3,4,5,6,0,
        1,1,1,1,1,1,1,1,1,1,1,1,7,
    },
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        2,2,2,2,2,2,2,2,2,2,2,2,2,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        2,2,2,9,9,9,9,9,9,9,9,9,9,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        3,3,3,3,3,3,3,3,3,3,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        9,9,9,9,9,9,9,9,9,9,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        4,4,4,4,4,4,4,4,4,4,4,4,4,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        4,4,4,9,9,9,9,9,9,9,9,9,9,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        5,5,5,5,5,5,5,5,5,5,5,5,5,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        9,9,9,9,9,9,9,9,9,9,5,5,5,
    },
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,2,3,4,1,5,0,6,7,8,2,3,0,
        0,3,4,1,5,4,0,7,8,2,3,1,0,
        0,4,1,5,4,6,0,8,2,3,1,5,0,
        0,1,5,4,6,7,0,2,3,1,5,6,0,
        0,5,4,6,7,8,0,3,1,5,6,7,0,
        0,4,6,7,8,2,0,1,5,6,7,8,0,
        0,6,7,8,2,5,0,5,6,7,8,2,0,
        0,7,8,2,5,1,0,6,7,8,2,3,0,
        0,8,2,5,1,6,0,7,8,2,3,4,0,
        0,2,5,1,6,7,0,8,2,3,4,1,0,
        0,5,1,6,7,8,0,2,3,4,1,5,0,
        0,1,6,7,8,2,0,3,4,1,5,6,0,
        0,6,7,8,2,3,0,4,1,5,6,7,0,
        0,7,8,2,3,4,0,1,5,6,7,8,0,
    },
#if 0
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,7,0,0,0,0,0,7,0,0,0,
        0,0,0,7,0,0,0,0,0,7,0,0,0,
        0,0,0,0,7,0,0,0,7,0,0,0,0,
        0,0,0,0,7,0,0,0,7,0,0,0,0, 
        0,0,0,1,1,1,1,1,1,1,0,0,0,
        0,0,0,1,1,1,1,1,1,1,0,0,0,
        0,0,1,1,5,1,1,1,5,1,1,0,0,
        0,0,1,1,5,1,1,1,5,1,1,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,1,1,1,1,1,0,
        0,1,0,1,1,1,1,1,1,1,0,1,0,
        0,1,0,1,0,0,0,0,0,1,0,1,0,
        0,1,0,1,0,0,0,0,0,1,0,1,0,
        0,0,0,0,1,1,0,1,1,0,0,0,0,
        0,0,0,0,1,1,0,1,1,0,0,0,0,
    },
#endif
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,9,0,9,0,9,0,9,0,0,0,
        0,9,0,0,0,0,0,0,0,0,0,9,0,
        0,9,9,0,9,0,0,0,9,0,9,9,0,
        0,0,0,0,0,0,2,0,0,0,0,0,0,
        0,9,0,0,0,9,3,9,0,0,0,9,0,
        0,0,0,9,0,0,4,0,0,9,0,0,0,
        0,0,0,0,0,0,5,0,0,0,0,0,0,
        0,0,0,9,0,0,6,0,0,9,0,0,0,
        0,9,0,0,0,9,7,9,0,0,0,9,0,
        0,0,0,0,0,0,1,0,0,0,0,0,0,
        0,9,9,0,9,0,0,0,9,0,9,9,0,
        0,9,0,0,0,0,0,0,0,0,0,9,0,
        0,0,0,9,0,9,0,9,0,9,0,0,0,
    },
};

constexpr  std::uint16_t BALL_CLR = 0x07E0;
//
}


Breakout::Breakout()
        : goblib::App<AppClock, MAX_FPS, MAX_FPS>()
        , goblib::Singleton<Breakout>()
        , _lcd(nullptr)
        , _sprites{}
        , _input()
        , _lcd_width(0)
        , _lcd_height(0)
        , _flip(false)
        , _phase(Phase::Start)
        , _balls()
        , _paddle()
        , _bricks()
        , _score()
        , _remain(REMAIN)
        , _stage(0)
        , _cnt(0)
{
    rewindBall();
    _bricks.set(stage[_stage].begin(), stage[_stage].end());
}

void Breakout::setup(LGFX* lcd)
{
    assert(lcd);
    _lcd = lcd;

    _lcd->init();
    _lcd->setBrightness(64);

    if(_lcd->width() < _lcd->height())
    {
        _lcd->setRotation(_lcd->getRotation() ^ 1);
    }
    _lcd_width = _lcd->width();
    _lcd_height = _lcd->height();

    uint32_t div = 2;
    sprite_height = (_lcd_height + div - 1) / div;
    for (;;)
    {
        bool fail = false;
        for (std::uint32_t i = 0; !fail && i < 2; ++i)
        {
            _sprites[i].setColorDepth(_lcd->getColorDepth());
            _sprites[i].setFont(&fonts::Font2);
            _sprites[i].setTextColor(0xFFFFFFU);
            fail = !_sprites[i].createSprite(_lcd_width, sprite_height);
        }
        if (!fail) break;
        for (std::uint32_t i = 0; i < 2; ++i)
        {
            _sprites[i].deleteSprite();
        }
        ++div;
    }
    _lcd->startWrite();

    _input.setup();

    PRINTF("FIELD HIT (%d,%d) - (%d,%d)  %d,%d\n",
           FIELD_HIT_RECT.left(), FIELD_HIT_RECT.top(), FIELD_HIT_RECT.right(), FIELD_HIT_RECT.bottom(),
           FIELD_HIT_RECT.width(), FIELD_HIT_RECT.height());
}

void Breakout::rewindBall()
{
    _balls.clear();
    _balls.emplace_back(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS * 2, BALL_CLR);
}

void Breakout::fixedUpdate()
{
    /* nop */
}

void Breakout::update(float delta)
{
    switch(_phase)
    {
    case Phase::Start: phaseStart(); break;
    case Phase::Game:  phaseGame();  break;
    case Phase::Clear: phaseClear(); break;
    case Phase::Miss:  phaseMiss();  break;
    }
    ++_cnt;
}

void Breakout::phaseStart()
{
    if(_cnt > MAX_FPS * 3) { _cnt = 0; _phase = Phase::Game; }
}

void Breakout::phaseGame()
{
    if(_bricks.isClear())
    {
        _cnt = 0;
        _phase = Phase::Clear;
        return;
    }

    auto it = std::remove_if(_balls.begin(), _balls.end(),
                             [](Ball& b)
                             {
                                 return b.center().y() > FIELD_HIT_RECT.bottom();
                             });
    _balls.erase(it, _balls.end());
    if(_balls.empty())
    {
        --_remain;
        _cnt = 0;
        _phase = Phase::Miss;
        return;
    }
    
    M5.update();
    _input.pump();

    // 0x01:left 0x02:rapid 0x04:right
    std::uint8_t mbit = (M5.BtnA.isPressed()) | (M5.BtnB.isPressed() << 1) | (M5.BtnC.isPressed() << 2) |
                        (_input.isPressed(goblib::m5s::FaceGB::Button::Left) ? 1 : 0) |
                        (_input.isPressed(goblib::m5s::FaceGB::Button::Right) ? 4 : 0) |
                        (_input.isPressed(goblib::m5s::FaceGB::Button::B) ? 2 : 0);

    if(mbit & 0x02)
    {
        for(auto& e : _balls)
        {
            if(e.ready()) { e.launch(); }
        }
    }

    std::int16_t ox = 0;
    if(mbit & 0x01) { ox -= 4 + 4 * ((mbit & 0x02) != 0); }
    if(mbit & 0x04) { ox += 4 + 4 * ((mbit & 0x02) != 0); }
    _paddle.offset(ox, 0);

    for(auto& e: _balls)
    {
        e.update(_paddle, _bricks);
    }

#if 0
    // Split ball
    if(_input.wasPressed(goblib::m5s::FaceGB::Button::Start))
    {
        _balls.emplace_back(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS * 2, Vec2(1.0f,-1.0f), 4.0f, BALL_CLR);
        _balls.back().launch();
    }
#endif
}

void Breakout::phaseClear()
{
    if(_cnt > MAX_FPS * 3)
    {
        _cnt = 0;
        _paddle.rewind();
        rewindBall();
        _stage = (_stage + 1) % stage.size();
        _bricks.set(stage[_stage].begin(), stage[_stage].end());
        
        _phase = Phase::Start;
    }
}

void Breakout::phaseMiss()
{
    if(_remain > 0 && _cnt > MAX_FPS * 3)
    {
        _cnt = 0;
        _phase = Phase::Game;
        _paddle.rewind();
        rewindBall();
    }
}

void Breakout::render()
{
    for (int_fast16_t y = 0; y < _lcd_height; y += sprite_height)
    {
        _flip = !_flip;
        goblib::lgfx::GSprite* s = &_sprites[_flip];

        s->clear();

        s->drawRect(FIELD_RECT.left(), FIELD_RECT.top() - y,
                    FIELD_RECT.width(), FIELD_RECT.height(), 0xFFFF);
#ifdef DEBUG_RENDER
        s->drawRect(FIELD_HIT_RECT.left(), FIELD_HIT_RECT.top() - y,
                    FIELD_HIT_RECT.width(), FIELD_HIT_RECT.height(), 0xF800);
#endif
        _bricks.render(s, y);
        _paddle.render(s, y);
        for(auto& e : _balls) { e.render(s, y); }
        
        switch(_phase)
        {
        case Phase::Start: renderStart(s, y); break;
        case Phase::Game:  renderGame(s, y);  break;
        case Phase::Clear: renderClear(s, y); break;
        case Phase::Miss:  renderMiss(s, y);  break;
        }
        if (y == 0)
        {
            s->setCursor(0, 0);
            s->printf("fps:%2.2f  SCORE: %08u REMAIN:%d", fps(), _score, _remain);
        }
        _sprites[_flip].pushSprite(_lcd, 0, y);
    }
    _lcd->display();
}

void Breakout::renderStart(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
    s->setCursor(80, 200 - yoffset);
    s->printf("STAGE %d READY?", _stage + 1);
}

void Breakout::renderGame(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
}

void Breakout::renderClear(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
    s->setCursor(80, 200 - yoffset);
    s->printf("STAGE %d CLEAR!", _stage + 1);
}

void Breakout::renderMiss(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
    s->setCursor(80, 200 - yoffset);
    s->printf("%s", _remain == 0 ? "GAME OVER!!" : "MISS!");
}
