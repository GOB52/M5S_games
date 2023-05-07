/*!
  Breakout
  @brief Simple breakout game
*/
#if __has_include (<M5Unified.h>)
# include <SdFat.h>
# include <M5Unified.h>
#else
# include <M5stack.h>
# ifdef min
#   undef min
# endif
#endif
#include "app.hpp"
#include "breakout.hpp"
#include "sound.hpp"
#include <gob_m5s_sd.hpp>
#define GOBLIB_ENABLE_PROFILE
#include <gob_profile.hpp>

extern const ::lgfx::GFXfont myfont_font; // myfont.cpp

namespace
{
int_fast16_t sprite_height;
constexpr std::uint32_t SPLIT = 6;

using StageData = std::vector<std::uint8_t>;
std::vector<StageData> stage  =
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
        1,1,1,1,1,1,1,1,1,1,1,1,1,
        9,9,1,1,9,9,1,1,9,9,1,9,9,
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
#if 1
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

bool createFromBitmap(LGFX_Sprite& sprite,const char* bitmap_path)
{
    goblib::m5s::File file;
    file.open(bitmap_path, O_READ);
    if(!file) { return false; }

    std::size_t len = file.available();
    if(len == 0) { return false; }

    auto bmp = new std::uint8_t[len];
    if(!bmp) { return false; }

    bool b = false;
    if(len == file.read(bmp, len))
    {
        sprite.createFromBmp(bmp, len);
        b = true;
    }
    delete[] bmp;
    return b;
}
//
}

Breakout::Breakout()
        : goblib::App<AppClock, MAX_FPS, MAX_FPS>()
        , goblib::Singleton<Breakout>()
        , _lcd(nullptr)
        , _sprites{}
        , _image()
        , _input()
        , _lcd_width(0)
        , _lcd_height(0)
        , _flip(false)
        , _phase(Phase::Start)
        , _balls()
        , _paddle(_image)
        , _bricks(_image)
        , _score()
        , _remain(REMAIN)
        , _stage(0)
        , _cnt(0)
{
    rewindBall();
    _bricks.set(stage[_stage].begin(), stage[_stage].end());
}

#ifdef M5UNIFIED_VERSION
void Breakout::setup(M5GFX* lcd)
#else
void Breakout::setup(LGFX* lcd)
#endif
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

    sprite_height = (_lcd_height + SPLIT - 1) / SPLIT;
    bool fail = false;
    for (;;)
    {
        for (std::uint32_t i = 0; !fail && i < 2; ++i)
        {
            _sprites[i].setColorDepth(_lcd->getColorDepth());
            _sprites[i].setFont(&myfont_font); // apply myfont
            _sprites[i].setTextColor(0xFFFFFFU);
            fail = !_sprites[i].createSprite(_lcd_width, sprite_height);
            _sprites[i].setAddrWindow(0, 0, _lcd_width, sprite_height); // 
        }
        if (!fail) break;
        for (std::uint32_t i = 0; i < 2; ++i)
        {
            _sprites[i].deleteSprite();
        }
    }
    assert(!fail && "Failed create sprute");

    PRINTF("FIELD HIT (%d,%d) - (%d,%d)  %d,%d\n",
           FIELD_HIT_RECT.left(), FIELD_HIT_RECT.top(), FIELD_HIT_RECT.right(), FIELD_HIT_RECT.bottom(),
           FIELD_HIT_RECT.width(), FIELD_HIT_RECT.height());

    // load bitmap resource from SD
    //    while(SoundSystem::_using_dma) { delay(1); }
    bool b = createFromBitmap(_image, "/res/bo2/bo2.bmp");
    assert(b && "Fatal error");
    Ball::_sprite = &_image;

    _lcd->setAddrWindow(0, 0, _lcd->width(), _lcd->height());
    _lcd->startWrite();
    _input.setup();

    playBgm(BGM::StartGame);
}

void Breakout::rewindBall()
{
    _balls.clear();
    _balls.emplace_back(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS * 2);
}

void Breakout::fixedUpdate()
{
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
    if(_cnt > MAX_FPS * 3)
    {
        _cnt = 0;
        _phase = Phase::Game;
        playBgm(BGM::Bgm1);
    }
}

void Breakout::phaseGame()
{
    if(_bricks.isClear())
    {
        _cnt = 0;
        _phase = Phase::Clear;
        playBgm(BGM::ClearGame);
        return;
    }
    _bricks.pump();
    
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
        playBgm(BGM::Miss);
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
    if(_cnt > MAX_FPS * 5)
    {
        _cnt = 0;
        _paddle.rewind();
        rewindBall();
        _stage = (_stage + 1) % stage.size();
        _bricks.set(stage[_stage].begin(), stage[_stage].end());
        
        _phase = Phase::Start;
        playBgm(BGM::StartGame);
    }
}

void Breakout::phaseMiss()
{
    if(_remain > 0 && _cnt > MAX_FPS * 3)
    {
        _cnt = 0;
        _phase = Phase::Game;
        playBgm(BGM::Bgm1);
        _paddle.rewind();
        rewindBall();
    }
}

void Breakout::render()
{
    const std::size_t tlen = _lcd->width() * sprite_height;
    std::int_fast16_t y = 0;

    for(std::int_fast16_t i = 0; i < SPLIT; ++i)
    {
        _flip = !_flip;
        LGFX_Sprite* s = &_sprites[_flip];

        s->clear();

        _renderBG(s, y);
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
            s->printf("FPS:%2.2f  SCORE: %08u REMAIN:%d", fps(), _score, _remain);
            _lcd->endWrite(); // Release DMA BUS
            SoundSystem::instance().pump(); // Access SD (using DMA BUS for access)
            _lcd->startWrite(); // Occupy DMA BUS for LCD
        }
        _lcd->pushPixelsDMA(static_cast<::lgfx::swap565_t*>(s->getBuffer()), tlen);
        y += sprite_height;
    }
}

void Breakout::renderStart(LGFX_Sprite* s, std::int_fast16_t yoffset)
{
    s->setCursor(80, 200 - yoffset);
    s->printf("STAGE %d READY?", _stage + 1);
}

void Breakout::renderGame(LGFX_Sprite* s, std::int_fast16_t yoffset)
{
}

void Breakout::renderClear(LGFX_Sprite* s, std::int_fast16_t yoffset)
{
    s->setCursor(80, 200 - yoffset);
    s->printf("STAGE %d CLEAR!", _stage + 1);
}

void Breakout::renderMiss(LGFX_Sprite* s, std::int_fast16_t yoffset)
{
    s->setCursor(80, 200 - yoffset);
    s->printf("%s", _remain == 0 ? "GAME OVER!!" : "MISS!");
}


void Breakout::_renderBG(LGFX_Sprite* s, std::int_fast16_t yoffset)
{
    constexpr goblib::lgfx::CellRect lt(0,32, 8,8);
    constexpr goblib::lgfx::CellRect ct(8,32, 8,8);
    constexpr goblib::lgfx::CellRect rt(16,32, 8,8);
    constexpr goblib::lgfx::CellRect lc(0,40, 8,8);
    constexpr goblib::lgfx::CellRect cc(8,40, 8,8);
    constexpr goblib::lgfx::CellRect rc(16,40, 8,8);

    _image.pushCell(s, lt, FIELD_RECT.left() - lt.width(), FIELD_RECT.top() - lt.height() - yoffset, 0);
    _image.pushCell(s, rt, FIELD_RECT.right(), FIELD_RECT.top() - lt.height() - yoffset, 0);
    for(int x = FIELD_RECT.left(); x < FIELD_RECT.right(); x += ct.width())
    {
        _image.pushCell(s, ct, x, FIELD_RECT.top() - lt.height() - yoffset);
    }
    for(int y = FIELD_RECT.top(); y < FIELD_RECT.bottom(); y += lc.width())
    {
        _image.pushCell(s, lc, FIELD_RECT.left() - 8, y - yoffset);
        _image.pushCell(s, rc, FIELD_RECT.right(), y - yoffset);
        for(int x = FIELD_RECT.left(); x < FIELD_RECT.right(); x += ct.width())
        {
            _image.pushCell(s, cc, x, y - yoffset);
        }
    }
}

void Breakout::playBgm(BGM bgm)
{
    _lcd->endWrite();
    SoundSystem::instance().playBgm(bgm);
    _lcd->startWrite();
}
