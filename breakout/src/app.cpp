/*!OA
  Breakout
  @brief Simple breakout game
*/
#include <M5Stack.h>
#ifdef min
#undef min
#endif
#include <LovyanGFX.hpp>
#include "app.hpp"
#include "breakout.hpp"

namespace
{
int_fast16_t sprite_height;
Paddle paddle;
std::vector<Ball*> balls;
Bricks bricks =
{
#if 0
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,0,1,0,1,0,1,0,1,0,1,0,1,
    0,1,0,1,0,1,0,1,0,1,0,1,0,
    1,0,1,0,1,0,1,0,1,0,1,0,1,
    0,1,0,1,0,1,0,1,0,1,0,1,0,
    1,0,1,0,1,0,1,0,1,0,1,0,1,
    0,1,0,1,0,1,0,1,0,1,0,1,0,
    1,0,1,0,1,0,1,0,1,0,1,0,1,
    0,1,0,1,0,1,0,1,0,1,0,1,0,
#endif

#if 0
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

#endif

#if 0
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,9,9,9,9,9,9,9,9,9,9,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,9,9,9,9,9,9,9,9,9,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,9,9,9,9,9,9,9,9,9,9,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,9,9,9,9,9,9,9,9,9,1,1,1,

#endif
#if 1    
    //    0,0,0,0,0,0,0,0,0,0,0,0,0,
    //    0,0,0,0,0,0,0,0,0,0,0,0,0,
    //    0,0,0,0,0,0,0,0,0,0,0,0,0,
    //    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,1,1,1,1,1,0,
    //    0,1,1,1,1,1,0,1,1,1,1,1,0,
    //    0,1,1,1,1,1,0,1,1,1,1,1,0,
    //    0,1,1,1,1,1,0,1,1,1,1,1,0,
#endif
};





constexpr  std::uint16_t CLR_1 = 0xF800;
constexpr  std::uint16_t CLR_2 = 0x07E0;
constexpr  std::uint16_t CLR_3 = 0x001F;
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
#ifdef BEHAVIOR_TEST
        , _cnt(0)
#endif

{
    //    balls.emplace_back(new Ball_1(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS, 0xFFFF));

    //    balls.emplace_back(new Ball_1(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS, Vec2(1.0f,1.0f), 8.0f, CLR_1));
    balls.emplace_back(new Ball_2(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS * 2, Vec2(1.0f,1.0f), 4.0f, CLR_2));
    //    balls.emplace_back(new Ball_3(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS, Vec2(1.0f,1.0f), 8.0f, CLR_3));

    
    //    balls.emplace_back(new Ball_2(FIELD_HIT_RECT.right() - 1, Paddle::INITIAL_TOP - Ball::RADIUS * 4, Vec2(0.0f,1.0f), 1.0f, CLR_2));


    
    for(int i=0;i<0;++i)
    {
        balls.emplace_back(new Ball_2(Paddle::INITIAL_LEFT + Paddle::WIDTH/2, Paddle::INITIAL_TOP - Ball::RADIUS, CLR_2));
    }


    
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
    for (;;)
    {
        sprite_height = (_lcd_height + div - 1) / div;
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

void Breakout::fixedUpdate()
{
    /* nop */
}

void Breakout::update(float delta)
{
    M5.update();
    _input.pump();

    //    if(!_input.isPressed(goblib::m5s::FaceGB::Button::A)) { return; }

    
    // 0x01:left 0x02:rapid 0x04:right
    std::uint8_t mbit = (M5.BtnA.isPressed()) | (M5.BtnB.isPressed() << 1) | (M5.BtnC.isPressed() << 2) |
                        (_input.isPressed(goblib::m5s::FaceGB::Button::Left) ? 1 : 0) |
                        (_input.isPressed(goblib::m5s::FaceGB::Button::Right) ? 4 : 0) |
                        (_input.isPressed(goblib::m5s::FaceGB::Button::B) ? 2 : 0);

#ifdef BEHAVIOR_TEST
    if(M5.BtnA.pressedFor(1000))
    {
        for(auto& e : balls) { delete e; }
        balls.clear();
        paddle.rewind();
        _cnt = 0;
    }
    else
    {
        if(M5.BtnA.wasPressed()) { testBehavior(BallType::BALL_TYPE_1, _cnt++); }
        if(M5.BtnB.wasPressed()) { testBehavior(BallType::BALL_TYPE_2, _cnt++); }
        if(M5.BtnC.wasPressed()) { testBehavior(BallType::BALL_TYPE_3, _cnt++); }
    }
    return;
#endif

    if(mbit & 0x02)
    {
        for(auto& e : balls)
        {
            if(e->ready()) { e->launch(); }
        }
    }

    std::int16_t ox = 0;
    if(mbit & 0x01) { ox -= 4 + 4 * ((mbit & 0x02) != 0); }
    if(mbit & 0x04) { ox += 4 + 4 * ((mbit & 0x02) != 0); }
#if 1
    paddle.offset(ox, 0);
#else
    static int oox = 4;
    if(paddle.rect().left() <= FIELD_RECT.left()) { oox = -oox; }
    if(paddle.rect().right() >= FIELD_RECT.right() - 1) { oox = -oox; }
    //    printf("%d:%d %d\n", paddle.rect().right(), FIELD_RECT.right(), oox);
    paddle.offset(oox, 0);
#endif

    for(auto& e: balls)
    {
        e->update(paddle, bricks);
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
        s->drawRect(FIELD_HIT_RECT.left(), FIELD_HIT_RECT.top() - y,
                    FIELD_HIT_RECT.width(), FIELD_HIT_RECT.height(), 0xF800);

        paddle.render(s, y);
        bricks.render(s, y);
        for(auto& e : balls)
        {
            e->render(s, y);
        }

        if (y == 0)
        {
            s->setCursor(0, 0);
#ifdef BEHAVIOR_TEST
            s->printf("fps:%2.2f balls:%zu cnt:%d", fps(), balls.size(), _cnt);
#else
            s->printf("fps:%2.2f", fps());
#endif
        }
        _sprites[_flip].pushSprite(_lcd, 0, y);
    }
    _lcd->display();
}


#ifdef BEHAVIOR_TEST
Ball* Breakout::createBall(const BallType type, const std::int16_t x, const std::int16_t y, const Vec2& v, const float vel)
{
    constexpr static std::uint16_t clr[] = { CLR_1, CLR_2, CLR3 };
    switch(type)
    {
    case BallType::BALL_TYPE_2:
        return new Ball_2(x, y, v, vel, clr[type]);
    case BallType::BALL_TYPE_3:
        return new Ball_3(x, y, v, vel, clr[type]);
        //    case BallType::BALL_TYPE_1:
    default:
        return new Ball_1(x, y, v, vel, clr[type]);
    }
}

void Breakout::testBehavior(const BallType type, const std::int32_t idx)
{
    paddle.rewind();
    auto pr = paddle.hitRect();

    for(auto& e : balls) { delete e; }
    balls.clear();
    
    switch(idx)
    {
    case 0:
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.top() + 10, Vec2(-1.0f, -0.25f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.top() + 10, Vec2(-1.0f, -0.5f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.top() + 10, Vec2(-1.0f, -0.75f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.top() + 10, Vec2(-1.0f, -1.0f), 40.0f));
        break;
    case 1:    
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.top() + 10, Vec2( 1.0f, -0.25f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.top() + 10, Vec2( 1.0f, -0.5f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.top() + 10, Vec2( 1.0f, -0.75f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.top() + 10, Vec2( 1.0f, -1.0f), 40.0f));
        break;
    case 2:
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.bottom() - 10, Vec2(-1.0f, 0.25f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.bottom() - 10, Vec2(-1.0f, 0.5f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.bottom() - 10, Vec2(-1.0f, 0.75f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 10, FIELD_HIT_RECT.bottom() - 10, Vec2(-1.0f, 1.0f), 40.0f));
        break;
    case 3:
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.bottom() - 10, Vec2( 1.0f, 0.25f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.bottom() - 10, Vec2( 1.0f, 0.5f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.bottom() - 10, Vec2( 1.0f, 0.75f), 40.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 10, FIELD_HIT_RECT.bottom() - 10, Vec2( 1.0f, 1.0f), 40.0f));
        break;
    case 4:
        // [A} from top,bottom
        printf("----[A] TB\n");
        balls.emplace_back(createBall(type, pr.center().x() - 2, pr.top() - 4,    Vec2( 1.0f,  1.0f), 10.0f));
        balls.emplace_back(createBall(type, pr.center().x() + 2, pr.bottom() + 4, Vec2(-1.0f, -1.0f), 10.0f));
        break;
    case 5:
        // [A] from left,right
        printf("----[A] LR\n");
        balls.emplace_back(createBall(type, pr.left() - 2,  pr.center().y() - 4, Vec2( 1.0f,  1.0f), 10.0f));
        balls.emplace_back(createBall(type, pr.right() + 2, pr.center().y() + 4, Vec2(-1.0f, -1.0f), 10.0f));
        break;
    case 6:
        // [B]
        printf("----[B]\n");
        balls.emplace_back(createBall(type, pr.center().x() - 2, pr.top() - 4,    Vec2( 0.1f,  1.0f), 25.0f));
        balls.emplace_back(createBall(type, pr.center().x() + 2, pr.bottom() + 4, Vec2(-0.1f, -1.0f), 25.0f));
        break;
    case 7:
        // [C]
        printf("----[C]\n");
        balls.emplace_back(createBall(type, pr.left() + 4, pr.top() + 4,     Vec2(  1.0f,  0.2f), 12.0f));
        balls.emplace_back(createBall(type, pr.right() - 4, pr.bottom() - 4, Vec2( -1.0f, -0.2f), 12.0f));
        break;
    case 8:
        // [D]
        printf("----[D] R\n");
        paddle.move(FIELD_RECT.right() - paddle.rect().width() - Ball::RADIUS * 4, paddle.rect().top());
        pr = paddle.hitRect();
        balls.emplace_back(createBall(type, pr.right() + 2, pr.top() - 2,    Vec2( 1.0f,  0.2f), 20.0f));
        balls.emplace_back(createBall(type, pr.right() + 2, pr.bottom() + 2, Vec2( 1.0f, -0.2f), 20.0f));
        break;
    case 9:
        // [D]
        printf("----[D] L\n");
        paddle.move(FIELD_RECT.left() + Ball::RADIUS * 4, paddle.rect().top());
        pr = paddle.hitRect();
        balls.emplace_back(createBall(type, pr.left() - 2, pr.top() - 2,    Vec2(-1.0f,  0.2f), 20.0f));
        balls.emplace_back(createBall(type, pr.left() - 2, pr.bottom() + 2, Vec2(-1.0f, -0.2f), 20.0f));
        break;
    case 10:
        // [F]
        printf("---[F] R\n");
        paddle.offset(300,0);
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 2,  pr.center().y(), Vec2( 1.0f,  0.1f), 10.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.right() - 2,  pr.center().y(), Vec2( 1.0f, -0.1f), 10.0f));
        break;
    case 11:
        // [F]
        printf("---[F] L\n");
        paddle.offset(-300,0);
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 2,  pr.center().y(), Vec2(-1.0f,  0.1f), 10.0f));
        balls.emplace_back(createBall(type, FIELD_HIT_RECT.left() + 2,  pr.center().y(), Vec2(-1.0f, -0.1f), 10.0f));
        break;
    }

    for(auto& e : balls)
    {
        e->launch();
        e->update(paddle, bricks);
    }
}
#endif
