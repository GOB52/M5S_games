
#include "breakout.hpp"

#include <LovyanGFX.hpp>
#include "breakout.hpp"
#include <lgfx/gob_lgfx.hpp>
#include <gob_math.hpp>
#include <gob_utility.hpp>
#include <gob_template_helper.hpp>
#include <gob_m5s_random.hpp>
#include <limits>
#include <algorithm>
#include <vector>

using Tangle = std::int8_t;
namespace
{


constexpr int TANGLE_DIV = 32;
//constexpr int SPLIT_WH = (Bricks::Brick::HEIGHT + Ball::RADIUS) / Ball::RADIUS;
constexpr int SPLIT_W = 5;
constexpr int SPLIT_H = 6;
constexpr Tangle tatan2(std::size_t v)
{
    //    return goblib::math::round( goblib::math::rad2deg(std::atan2(v / SPLIT_W, v % SPLIT_W)) / (360 / TANGLE_DIV) );
    return goblib::math::round( goblib::math::rad2deg(std::atan2(v / 10, v % 10)) / (360 / TANGLE_DIV) );
    // atan2 使わない方法...
    // 31 & 
}
constexpr auto atan2_table2 = goblib::template_helper::table::generator<10 * 6>(tatan2);


constexpr float tsin(int x) { return std::sin(goblib::math::deg2rad(x*360.0f/TANGLE_DIV)); }
constexpr auto sin_table = goblib::template_helper::table::generator<TANGLE_DIV>(tsin);
constexpr float tcos(int x) { return std::cos(goblib::math::deg2rad(x*360.0f/TANGLE_DIV)); }
constexpr auto cos_table = goblib::template_helper::table::generator<TANGLE_DIV>(tcos);



#if 0
constexpr std::array<Tangle, 6*6> atan2_table =
{
    -1,0,0,0,0,0,
    8,4,2,1,1,1,
    8,6,4,2,2,1,
    8,7,6,4,3,2,
    8,7,6,5,4,3,
    8,7,7,6,5,4,
};
#else
constexpr std::array<Tangle, SPLIT_W * SPLIT_H> atan2_table =
{
#if 1
   -1,0,0,0,0,
    8,4,2,1,1,
    8,6,4,2,2,
    8,7,6,4,2,
    8,7,6,5,5,
    //    8,7,6,4,3,
    8,7,6,5,4,
#else
   -1,0,0,0,0,0,0,0,
    8,4,2,1,1,1,0,0,
    8,6,4,2,2,1,1,1,
    8,7,6,4,3,2,2,2,
    8,7,6,5,4,3,2,2,
    8,7,7,6,5,4,3,3,
    8,8,7,6,6,5,4,3,
    8,8,7,6,6,5,5,4,
    #endif
};
#endif


constexpr std::array<Tangle, 9 * 6> paddle_table =
{
#if 0
    -1, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 4, 2, 1, 1, 1, 0, 0, 0,
    8, 6, 4, 2, 2, 1, 1, 1, 1,
    8, 7, 6, 4, 3, 2, 2, 2, 2,
    8, 7, 6, 5, 4, 3, 2, 2, 2,
    8, 7, 7, 6, 5, 4, 3, 3, 2,
#else
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    7, 4, 2, 1, 1, 1, 1, 1, 1,
    7, 6, 4, 2, 2, 1, 1, 2, 2,
    7, 7, 6, 4, 3, 2, 2, 2, 2,
    7, 7, 6, 5, 4, 3, 2, 3, 3,
    7, 7, 6, 6, 5, 4, 4, 3, 3,
#endif

};

Tangle table_paddle_atan2(const std::int16_t y, const std::int16_t x)
{
    assert(std::abs(y)*9 + std::abs(x/2) < paddle_table.size() && "Illegal size");
    auto v = paddle_table[ std::abs(y) * SPLIT_W + std::abs(x/2) ];
    if(x < 0) { v = (TANGLE_DIV/2) - v; }
    if(y < 0) { v = TANGLE_DIV - v; }
    return v;
}



Tangle table_atan2(const std::int16_t y, const std::int16_t x)
{
    //    if(y == x && y == 0) { return Tangle(-1); }

    //   printf(" -> atan2:(%d,%d) -> [%d,%d]\n", x,y, xx, yy);
   //   assert(std::abs(xx) < SPLIT_WH && "Ilegal xx");
   //   assert(std::abs(yy) < SPLIT_WH && "Ilegal yy");

    //    auto xx = (x + goblib::math::sign(x)) / 2; // ( x ) / 2;
    auto xx = x/2;
    auto yy = y;
    assert(std::abs(yy)*SPLIT_W + std::abs(xx) < atan2_table.size() && "Illegal size");
    
    auto v = atan2_table[ std::abs(yy) * SPLIT_W + std::abs(xx) ];
    if(xx < 0) { v = (TANGLE_DIV/2) - v; }
    if(yy < 0) { v = TANGLE_DIV - v; }

    return v;
}

// 32/(PI*2) = 5.09295f
// #define ATAN_TO_ATAN2_FACTOR (TANGLE_DIV/(2*goblib::math::constants::pi2_f))


// Vecor of field rectanle (Clockwise)
// ^------>
// |      |
// |      |
// <------V
constexpr Vec2 FIELD_VECTOR[][2] =
{
    Vec2(FIELD_HIT_RECT.leftTop()),     Vec2(FIELD_HIT_RECT.rightTop()),    // top
    Vec2(FIELD_HIT_RECT.rightTop()),    Vec2(FIELD_HIT_RECT.rightBottom()), // right
#ifdef INCLUDE_HIT_BOTTOM_OF_FIELD
    Vec2(FIELD_HIT_RECT.rightBottom()), Vec2(FIELD_HIT_RECT.leftBottom()),  // bottom
#endif
    Vec2(FIELD_HIT_RECT.leftBottom()),  Vec2(FIELD_HIT_RECT.leftTop()),     // left
};

/*!
  Detecting intersections and calculating reflection points.
  @param as start of entering vector
  @param ae end of entering vector
  @param bs start of target vector
  @param be end of target vector
  @param[out] rpos reflection point if exists
  @param[out] cpos crossing point if exists
  @retval true Crossing
  @retval false No crossing
 */
bool intersection(const Vec2& as, const Vec2& ae, const Vec2& bs, const Vec2& be, Vec2* rpos = nullptr, Vec2* cpos = nullptr)
{
    auto v1 = ae - as;
    auto v2 = be - bs;
    auto cv1v2 = v1.cross(v2);

    if(cv1v2 == 0.0f) { return false; } // parallel?
    
    auto v = bs - as;
    auto t1 = v.cross(v2) / cv1v2;
    auto t2 = v.cross(v1) / cv1v2;

    // No crossing?
    if(std::isless(t1, 0.0f) || std::isgreater(t1, 1.0f) ||
       std::isless(t2, 0.0f) || std::isgreater(t2, 1.0f))
    {
        return false;
    }

    // Calculate crossing point and reflection point
    auto cpoint = as + (v1 * t1);
    if(cpos) { *cpos = cpoint; }
    if(rpos) {
        *rpos = v1.reflectionV(v2.perpendicularV().normalizeV()) * (1.0f - t1) + cpoint;
    }

    if(cpos && rpos)
    {
#ifndef NDEBUG
        if(std::isnan(cpos->x()) || std::isnan(cpos->y()) || std::isnan(rpos->x()) || std::isnan(rpos->y()))
        {
            PRINTF("    IS>T:%f,%f (%f,%f)-(%f,%f) t:%f,%f\n", t1, t2, as.x(),as.y(), ae.x(),ae.y(), t1, t2);
        }
        assert(!std::isnan(cpos->x()) && !std::isnan(cpos->y()) && "CPOS nan");
        assert(!std::isnan(rpos->x()) && !std::isnan(rpos->y()) && "RPOS nan");
#endif
    }

    return true;
}

// Is pos left of bs - be vector?
bool isLeft(const Vec2& pos, const Vec2& bs, const Vec2& be)
{
    return std::isless((be - bs).cross(pos - bs), 0.0f);
}
// Is pos right of bs - be vector?
bool isRight(const Vec2& pos, const Vec2& bs, const Vec2& be)
{
    return std::isgreater((be - bs).cross(pos - bs), 0.0f);
}
// Is Rect(int) contains Vec2(float)?
bool contains(const Rect2& r, const Vec2& pos)
{
    return std::isgreaterequal(pos.x(), r.left()) && std::islessequal(pos.x(), r.right()) &&
            std::isgreaterequal(pos.y(), r.top()) && std::islessequal(pos.y(), r.bottom());
}
//
}

// ----------------------------------------------------------------------------
// Paddle
// ----------------------------------------------------------------------------
Paddle::Paddle()
        : _rect(INITIAL_LEFT, INITIAL_TOP, WIDTH, HEIGHT)
        , _hitRect(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS, _rect.width() + Ball::RADIUS * 2, _rect.height() + Ball::RADIUS * 2)
        , _prev(_rect.leftTop())
{
    PRINTF("PADDLE: r(%d,%d)[%d,%d] hr(%d,%d)[%d,%d]\n",
           _rect.left(), _rect.top(), _rect.width(), _rect.height(),
           _hitRect.left(), _hitRect.top(), _hitRect.width(), _hitRect.height());

}
#if 0
[ 0] 00000000 8(0,0), 4(0,0) 16(0,0)  X
[ 1] 00000001 8(0,1), 4(0,1) 16(0,1)  X
[ 2] 00000010 8(0,2), 4(0,2) 16(0,2)  X
[ 3] 00000011 8(0,3), 4(0,3) 16(0,3)  X

        [ 4] 00000100 8(0,4), 4(1,0) 16(0,4)  XY
[ 5] 00000101 8(0,5), 4(1,1) 16(0,5)  Y
[ 6] 00000110 8(0,6), 4(1,2) 16(0,6)  Y
[ 7] 00000111 8(0,7), 4(1,3) 16(0,7)  Y
[ 8] 00001000 8(1,0), 4(2,0) 16(0,8)  Y
[ 9] 00001001 8(1,1), 4(2,1) 16(0,9)  Y
[10] 00001010 8(1,2), 4(2,2) 16(0,10) Y
[11] 00001011 8(1,3), 4(2,3) 16(0,11) Y

        [12] 00001100 8(1,4), 4(3,0) 16(0,12) -XY
[13] 00001101 8(1,5), 4(3,1) 16(0,13) -X
[14] 00001110 8(1,6), 4(3,2) 16(0,14) -X
[15] 00001111 8(1,7), 4(3,3) 16(0,15) -X
[16] 00010000 8(2,0), 4(4,0) 16(1,0) -X
[17] 00010001 8(2,1), 4(4,1) 16(1,1) -X
[18] 00010010 8(2,2), 4(4,2) 16(1,2) -X
[19] 00010011 8(2,3), 4(4,3) 16(1,3) -X

        [20] 00010100 8(2,4), 4(5,0) 16(1,4)  -X-Y
[21] 00010101 8(2,5), 4(5,1) 16(1,5)  -Y
[22] 00010110 8(2,6), 4(5,2) 16(1,6)  -Y
[23] 00010111 8(2,7), 4(5,3) 16(1,7) -Y
[24] 00011000 8(3,0), 4(6,0) 16(1,8) -Y
[25] 00011001 8(3,1), 4(6,1) 16(1,9) -Y
[26] 00011010 8(3,2), 4(6,2) 16(1,10) -Y
[27] 00011011 8(3,3), 4(6,3) 16(1,11) -Y

        [28] 00011100 8(3,4), 4(7,0) 16(1,12) X-Y
[29] 00011101 8(3,5), 4(7,1) 16(1,13) X
[30] 00011110 8(3,6), 4(7,2) 16(1,14) X
[31] 00011111 8(3,7), 4(7,3) 16(1,15) X
#endif


void Paddle::rewind()
{
    _rect.move(INITIAL_LEFT, INITIAL_TOP);
    _hitRect.move(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS);
}

#ifdef BEHAVIOR_TEST
void Paddle::move(std::int16_t x, std::int16_t y)
{
    _rect.move(x, y);
    _hitRect.move(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS);
}
#endif

void Paddle::offset(std::int16_t ox, std::int16_t oy)
{
    _prev = _rect.leftTop();
    // clamp in FIELD_RECT
    auto lt = _rect.leftTop();
    _rect.move( goblib::clamp(lt.x() + ox, (int)FIELD_RECT.left(), FIELD_RECT.right() - WIDTH),
                goblib::clamp(lt.y() + oy, (int)FIELD_RECT.top(),  FIELD_RECT.bottom() - HEIGHT) );
    _hitRect.move(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS);
}

void Paddle::render(goblib::lgfx::GSprite* s, const std::int_fast16_t yoffset)
{
#ifndef ONLY_HIT_FIELD
       s->fillRect(_rect.left(), _rect.top() - yoffset, _rect.width(), _rect.height(), 0xFFFF /* White(255,255,255) */);
       s->drawRect(_hitRect.left(), _hitRect.top() - yoffset, _hitRect.width(), _hitRect.height(), 0xF800);
#endif
}

// ----------------------------------------------------------------------------
// Bricks
// ----------------------------------------------------------------------------
Bricks::Brick::Brick(std::int16_t x, std::int16_t y, std::uint8_t type)
        : _type(type)
        , _life(type != 0)
        , _rect(x, y, WIDTH, HEIGHT)
        , _hitRect(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS, _rect.width() + Ball::RADIUS * 2, _rect.height() + Ball::RADIUS * 2)
{}

void Bricks::Brick::render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
    // color for _type
    constexpr static std::uint16_t clr[] = { 0x000, TFT_DARKGREY, TFT_BLUE, TFT_GREEN, TFT_CYAN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_LIGHTGREY, TFT_OLIVE };
    if(alive())
    {
        s->fillRect(_rect.left(), _rect.top() - yoffset, _rect.width(), _rect.height(), clr[_type]);
        s->drawRect(_rect.left(), _rect.top() - yoffset, _rect.width(), _rect.height(), 0xFFFF);
        s->drawRect(_hitRect.left(), _hitRect.top() - yoffset, _hitRect.width(), _hitRect.height(), 0xF800);
    }
}

bool Bricks::isClear() const
{
    return std::all_of(_array.begin(), _array.end(), [](const Brick& e) { return !e.alive(); });
}

Bricks::Bricks(std::initializer_list<std::uint8_t> init)
{
    int x = 0;
    int y = 0;
    for(auto& e: init)
    {
        if(e)
        {
            _array.emplace_back(Brick(LEFT + Brick::WIDTH * x, TOP + Brick::HEIGHT * y, e));
        }
        if(++x >= NUM_OF_HORIZONTAL)
        {
            x = 0;
            ++y;
        }
    }

        //03:11:11.309 > hit bricks 1 ball(197,84) ***
    //block (182,74) - (201, 85)
    //block (198,74) - (217, 85)
    Vec2 nv(197,84);
    int idx = 0;
    printf("-------------check\n");
    for(auto& e: _array)
    {
        auto br = e.hitRect();
        if(contains(br, nv))
        {
            printf("block [%d] (%d,%d) - (%d, %d)\n", idx, br.left(), br.top(), br.right(), br.bottom());
            int xx = nv.x() - br.center().x();
            int yy = nv.y() - br.center().y();
            auto angle = table_atan2(nv.y() - br.center().y(), nv.x() - br.center().x());
            printf("xy(%d,%d) hitBrick %d\n", xx,yy,angle);
        }
        ++idx;
    }
    printf("-------------end\n");

    auto br = _array[47].hitRect();;
    printf("block (%d,%d) - (%d, %d)\n",  br.left(), br.top(), br.right(), br.bottom());
    br = _array[48].hitRect();
    printf("block (%d,%d) - (%d, %d)\n",  br.left(), br.top(), br.right(), br.bottom());

}

Bricks::Brick* Bricks::get(std::int16_t x, std::int16_t y)
{
    auto xi = (x - LEFT) / Brick::WIDTH;
    auto yi = (y - TOP) / Brick::HEIGHT;
    size_t idx = yi * NUM_OF_HORIZONTAL + xi;
    return (xi < NUM_OF_HORIZONTAL && idx < _array.size()) ? &_array[idx] : nullptr;
}

Rect2 Bricks::bounding() const
{
    std::int16_t left(FIELD_HIT_RECT.right()), top(FIELD_HIT_RECT.bottom()), right(FIELD_HIT_RECT.left()), bottom(FIELD_HIT_RECT.top());

    for(auto& e : _array)
    {
        if(!e.alive()) { continue; }
        left = std::min(left, e.hitRect().left());
        right = std::max(right, e.hitRect().right());
        top = std::min(top, e.hitRect().top());
        bottom = std::max(bottom, e.hitRect().bottom());
    }
    return Rect2(left, top, right - left + 1, bottom - top + 1);
}

void Bricks::render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
#ifndef ONLY_HIT_FIELD
    for(auto& e : _array)
    {
        e.render(s, yoffset);
    }

    Rect2 br = bounding();
    if(br)
    {
        s->drawRect(br.left(), br.top() - yoffset, br.width(), br.height(), 0x001F);
    }
#endif
}

// ----------------------------------------------------------------------------
// Ball
// ----------------------------------------------------------------------------
void Ball::launch()
{
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    /*
03:11:11.309 > hit bricks 1 ball(197,84) ***
03:11:11.309 >   4,5 => angle 4
03:11:11.576 > hit bricks 1 ball(224,106)
03:11:11.576 >   -5,0 => angle 24
03:11:12.276 > hit bricks 2 ball(232,52)
03:11:12.276 >   4,0 => angle 8
03:11:12.543 > hit bricks 1 ball(207,74)
03:11:12.543 >   -5,0 => angle 24
03:11:12.810 > hit bricks 1 ball(181,52) ***
03:11:12.810 >   4,5 => angle 4
03:11:13.077 > hit bricks 1 ball(208,74)
03:11:13.077 >   -5,0 => angle 24
03:11:13.377 > hit bricks 1 ball(234,52)
03:11:13.377 >   4,-5 => angle 10
03:11:14.045 > hit bricks 1 ball(222,106)
03:11:14.045 >   -5,-1 => angle 24
03:11:14.311 > hit bricks 1 ball(197,84)
03:11:14.311 >   4,5 => angle 4
.... loop
    */




#if 0

    // 16,8
    Bricks::Brick brick(0,0,1);
    auto br = brick.hitRect(); // [20,12]
    auto br2 = brick.rect();
    
    printf("TABLE %zu\n", atan2_table.size());
    for(int i=0;i<atan2_table.size()/SPLIT_W;++i)
    {
        for(int x = 0; x < SPLIT_W; ++x)
        {
            printf("%2d,",atan2_table[i * SPLIT_W + x]);
        }
        printf("\n");
    }
    printf("TABLE2 %zu\n", atan2_table2.size());
    for(int i=0;i<atan2_table2.size()/10;++i)
    {
        for(int x = 0; x < 10; ++x)
        {
            printf("%2d,",atan2_table2[i * SPLIT_W + x]);
        }
        printf("\n");
    }


    printf("Brick R:(%d,%d) - (%d,%d) [%d,%d] H:(%d,%d) - (%d,%d) [%d,%d] C:(%d,%d)\n",
           br2.left(),br2.top(),br2.right(),br2.bottom(), br2.width(), br2.height(),
           br.left(),br.top(),br.right(),br.bottom(), br.width(), br.height(),
           br.center().x(), br.center().y());


    for(int y=0;y<=4; y+=2)
    {
        for(int x= 0;x<=8; x+=4)
        {
            printf("(%d,%d) : %d\n", x, y, table_atan2(y,x) );
        }
    }

    
    
    #if 0
    for(int y = -1; y <= 1; ++y)
    {
        for(int x = -1; x <= 1; ++x)
        {
            auto a = std::atan2(y,x);
            auto cx = std::cos(a);
            auto cy = std::sin(a);
            printf("(%d,%d) %f/%f [%f,%f]\n", x,y,a, goblib::math::rad2deg(a), cx,cy);
        }
    }
    #endif

    #if 0
    for(int y = br.top(); y <= br.bottom();++y)
    {
        for(int x = br.left(); x <= br.right(); ++x)
        {
            Vec2 v(x,y)
            int xx = v.x() - br.center().y();
            int yy = v.y() - br.center().y();
            xx += -goblib::math::sign(xx) * Ball::Radius;
            yy += -goblib::math::sign(yy) * Ball::Radius;
            auto a = table_atan2(yy, xx);
            printf("Vec(%d,%d) [%d,%d] angle:%d \n", x, y, xx, yy, a);
        }
    }
    #endif

    #if 0
    std::vector<Vec2> vv = { Vec2(-8,-8), Vec2(8, -2), Vec2(21,11), Vec2(-2,11) };
    for(auto& e : vv)
    {
        Vec2 v(e);
        int xx = v.x() - br.center().y();
        int yy = v.y() - br.center().y();
        int xx2 = xx + -goblib::math::sign(xx) * Ball::RADIUS;
        int yy2 = yy + -goblib::math::sign(yy) * Ball::RADIUS;

        auto a = table_atan2(yy2, xx2);
        printf("Vec(%d,%d) [%d,%d]=>[%d,%d] angle:%d \n", (int)v.x(), (int)v.y(), xx, yy, xx2, yy2, a);
    }
    #endif
    
    #if 0
    for(size_t i = 0; i < goblib::size(v); ++i)
    {
        auto nv = v[i];
        auto xd = std::abs(nv.x() - br.center().x());
        auto yd = std::abs(nv.y() - br.center().y()) * (Bricks::Brick::WIDTH / (float)Bricks::Brick::HEIGHT);

        int xx = nv.x() - br.center().x();
        int yy = nv.y() - br.center().y();
        //        printf("[%zu] HIT Brick %f,%f => %c | [%d,%d] %d\n", i, xd, yd, (xd < yd) ? 'Y' : 'X', xx,yy,table_atan2(yy,xx) );
        printf("[%zu] HIT Brick [%d,%d] %d\n", i, xx, yy, table_atan2(yy,xx) );

    }
    #endif


    #if 0
    for(int i=0;i<32;++i)
    {
        printf("[%d] %s 8(%d,%d), 4(%d,%d) 16(%d,%d) \n", i, std::bitset<8>(i).to_string().c_str(), i/8, i%8, i/4, i%4, i/16, i%16);
    }
    #endif
    return;
#endif


    if(_launch) { return; }
    
    _launch = true;

#ifndef BEHAVIOR_TEST
    if(_v.length() == 0.0f)
    {
        goblib::m5s::arduino_engine re;
        std::uniform_real_distribution<> distx(-1.0f, 1.0f);
        _v.move(distx(re), -1.0f);
        _v.normalize();
    }
#endif
    printf("BALL:launch %f,%f\n", _v.x(), _v.y());
}

void Ball::render(goblib::lgfx::GSprite* s, const std::int_fast16_t yoffset)
{
    s->fillCircle(_center.x(), _center.y() - yoffset, RADIUS, _color);

    if(!_history.empty())
    {
        for(size_t i = 0; i < _history.size() - 1; ++i)
        {
            s->drawLine(_history[i].x(),   _history[i].y() - yoffset,
                        _history[i+1].x(), _history[i+1].y() - yoffset, _color ^ 0xFFFF);
        }
    }
}

// ----------------------------------------------------------------------------
// Ball_1
// ----------------------------------------------------------------------------
void Ball_1::update(Paddle& paddle, Bricks& bricks)
{
    if(ready()) { return; }

    PRINTF("---- %d\n", id());

    assert(contains(FIELD_HIT_RECT, _center) && "OUT OF FIELD");
    
    _prev = _center;

    Vec2 ov = _center;
    Vec2 nv = _center + _v * velocity();

#ifdef BEHAVIOR_TEST    
    _history.push_back(nv);
#endif
    
    _update(ov, nv, paddle, bricks);
    _center = nv;

    PRINTF("UPDATE (%d,%d) - (%d,%d)\n", (int)_prev.x(), (int)_prev.y(), (int)nv.x(), (int)nv.y());
    
    _history.push_back(_center);
}

void Ball_1::_update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks)
{
    auto pr = paddle.hitRect();
    bool oin = contains(pr, ov);
    bool nin = contains(pr, nv);
    float ydir = goblib::math::sign(nv.y() - ov.y()) >= 0.0f ? 1.0f : -1.0f;

    PRINTF("update (%d,%d)-(%d,%d) PADDLE:(%d,%d)-(%d,%d)\n",
           (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y(),
           pr.left(), pr.top(), pr.right(), pr.bottom());

    // Check for bricks
#ifndef ONLY_HIT_FIELD
    if(contains(bricks.bounding(), ov))
    {
        bool hit = false;
        for(auto& e : bricks.array())
        {
            if(!e.alive()) { continue; }
            if(contains(e.hitRect(), nv))
            {
                PRINTF("[%d] HTI BRICK(%d,%d)\n", id(), e.rect().left(), e.rect().top() );
                e.hit();
                hit = true;
            }
        }
        if(hit)
        {
            _v.move(_v.x(), -_v.y());
            nv = ov + _v * velocity();
            PRINTF("==> [%d] HTI BRICK v:(%f,%f)\n", id(), _v.x(), _v.y());
        }
    }
#endif
    
    // Check for field
    if(!contains(FIELD_HIT_RECT, nv))
    {
        if(nv.x() < FIELD_HIT_RECT.left())
        {
            _v.move(std::abs(_v.x()), _v.y());
        }
        if(nv.x() > FIELD_HIT_RECT.right())
        {
            _v.move(-std::abs(_v.x()), _v.y());
        }
        if(nv.y() < FIELD_HIT_RECT.top())
        {
            _v.move(_v.x(), std::abs(_v.y()));
        }
#ifdef INCLUDE_HIT_BOTTOM_OF_FIELD
        if(nv.y() > FIELD_HIT_RECT.bottom())
        {
            _v.move(_v.x(), -std::abs(_v.y()));
        }
#endif
        PRINTF_S("HIT FIELD\n");
        nv = ov + _v * velocity(); // Apply reflection by wall of field
        nin = contains(pr, nv);
    }

#ifdef ONLY_HIT_FIELD
    return;
#endif
    
    // Check for paddle
    // [Collision C]
    if(oin && nin)
    {
        ov.move(ov.x(), (ydir > 0.0f) ? pr.top() - 1.0f : pr.bottom() + 1.0f);
        nv = ov + _v * velocity();
        oin = contains(pr, ov);
        nin = contains(pr, nv);
        PRINTF("[%d]C,F IN PADDLE (%d,%d)-(%d,%d)\n", id(), (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y());
    }
    // [Collision A]
    if(!oin && nin)
    {
        PRINTF("[%d]A HIT PADDLE\n", id());
        _v.move(_v.x(), -ydir * std::abs(_v.y()));
    }
    // [Collsion B]
    if(!oin && !nin)
    {
        Rect2 brect(goblib::math::fmin(ov.x(), nv.x()), goblib::math::fmin(ov.y(), nv.y()),
                    std::abs(ov.x() - nv.x()) + 1, std::abs(ov.y() - nv.y()) + 1);
        PRINTF("br:(%d,%d)-(%d,%d) [%d,%d]\n", brect.left(), brect.top(), brect.right(), brect.bottom(),brect.width(), brect.height());

        if(brect.overlaps(pr))
        {
            Vec2 pv[][2] =
            {
                { Vec2(pr.leftTop()),     Vec2(pr.rightTop()) },    // 0:upper
                { Vec2(pr.rightTop()),    Vec2(pr.rightBottom()) }, // 1:right
                { Vec2(pr.rightBottom()), Vec2(pr.leftBottom()) },  // 2:bottom
                { Vec2(pr.leftBottom()),  Vec2(pr.leftTop()) },     // 3:left
            };
            for(size_t i = 0; i < goblib::size(pv); ++i)
            {
                if(intersection(ov, nv, pv[i][0], pv[i][1]))
                {
                    PRINTF("[%d]B HIT PADDLE %zu\n", id(), i);
                    _v.move(_v.x(), -ydir * std::abs(_v.y()));
                    break;
                }
            }
        }
    }
    nv = ov + _v * velocity(); // Apply reflection by paddle or No crossing
}

// ----------------------------------------------------------------------------
// Ball_2
// ----------------------------------------------------------------------------
void Ball_2::update(Paddle& paddle, Bricks& bricks)
{
    if(ready()) { return; }

    PRINTF("---- %d\n", id());
    _prev = _center;

    //    assert(FIELD_HIT_RECT.contains(_center.x(), _center.y()) && "OUT OF FIELD");

    if(!FIELD_HIT_RECT.contains(_center.x(), _center.y()))
    {
        printf("(%d,%d)-(%d,%d) %f,%f\n",
               FIELD_HIT_RECT.left(), FIELD_HIT_RECT.top(), FIELD_HIT_RECT.right(), FIELD_HIT_RECT.bottom(),
               _center.x(), _center.y());
        assert(0);
    }

    
    auto cnt = velocity();
    Vec2 ov;
    Vec2 nv = _center;
    while(cnt--)
    {
        ov = nv;
        nv = ov + _v;
        _update(ov, nv, paddle, bricks);
        //        PRINTF("  _updated (%d,%d) - (%d,%d)\n", (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y());
    }

    _center = nv;

    PRINTF("UPDATE (%d,%d) - (%d,%d)\n", (int)_prev.x(), (int)_prev.y(), (int)nv.x(), (int)nv.y());
    
    _history.push_back(_center);
}

// move 1 step.
void Ball_2::_update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks)
{
    // Check for paddle
    if(contains(paddle.hitRect(), nv))
    {
#if 0
        float ydir = goblib::math::sign(nv.y() - ov.y()) >= 0.0f ? 1.0f : -1.0f;
        PRINTF("HIT PADDLE %f\n", ydir);
        //        ov.move(ov.x(), ydir > 0.0f ? paddle.hitRect().top() : paddle.hitRect().bottom());
        //        _v.move(_v.x(), -ydir * std::abs(_v.y())); // to top or bottom
        // 数学座標系とスクリーンが違うのでそのまま
        auto angle = std::atan2(nv.y() - paddle.rect().center().y(), nv.x() - paddle.rect().center().x());
        _v.move(std::cos(angle), std::sin(angle));
        _v.normalize();

        ov.move(ov.x(), ydir > 0.0f ? paddle.hitRect().top() - 1 : paddle.hitRect().bottom() + 1);

        auto angle2 = std::atan2(_v.y(), _v.x());
        printf("angle:%f/%f v:(%f,%f) %f/%f\n",
               angle, goblib::math::rad2deg(angle), _v.x(), _v.y(),
               angle2, goblib::math::rad2deg(angle2) );
#else
        auto pr = paddle.hitRect();
        int xd = nv.x() - pr.center().x();
        int yd = nv.y() - pr.center().y();
        auto angle = table_paddle_atan2(yd, xd);
        printf("HIT PADDLE diff:(%d,%d) a:%d\n", xd, yd, angle);
        _v.move(cos_table[angle], sin_table[angle]);
        //onHitPaddle();
#endif
    }

    // Check for field
    bool hitField = false;
    if(nv.x() < FIELD_HIT_RECT.left())
    {
        PRINTF_S("HIT FIELD L\n");
        _v.move(std::abs(_v.x()), _v.y());
        hitField = true;
    }
    if(nv.x() > FIELD_HIT_RECT.right())
    {
        PRINTF_S("HIT FIELD R\n");
        _v.move(-std::abs(_v.x()), _v.y());
        hitField = true;
    }
    if(nv.y() < FIELD_HIT_RECT.top())
    {
        PRINTF_S("HIT FIELD T\n");
        _v.move(_v.x(), std::abs(_v.y()));
        hitField = true;
    }
#ifdef INCLUDE_HIT_BOTTOM_OF_FIELD
    if(nv.y() > FIELD_HIT_RECT.bottom())
    {
        PRINTF_S("HIT FIELD B\n");
        _v.move(_v.x(), -std::abs(_v.y()));
        hitField = true;
    }
#endif
    if(hitField) { onHitWall(); }

    
#if 1
    if(contains(bricks.bounding(), nv))
    {
        std::vector<Bricks::Brick*> hit;
        for(auto& e : bricks.array())
        {
            if(!e.alive()) { continue; }
            if(contains(e.hitRect(), nv))
            {
                hit.push_back(&e);
            }
        }
        if(!hit.empty())
        {
            onHitBrick();
            printf("hit bricks %zu ball(%f,%f)\n", hit.size(), nv.x(), nv.y());
            Rect2 br;
            for(auto& e : hit) { br |= e->hitRect(); e->hit(); }
            auto angle = table_atan2(nv.y() - br.center().y(), nv.x() - br.center().x());
            printf("  %d,%d => angle %d\n",
                   (int)(nv.y() - br.center().y()),
                   (int)(nv.x() - br.center().x()),
                   angle);
            switch(angle)
            {
            case -1:
                _v.move(-_v.x(), -_v.y());
                break;
            case 4:
                _v.move( std::abs(_v.x()),  std::abs(_v.y()) );
                break;
            case 12:
                _v.move(-std::abs(_v.x()),  std::abs(_v.y()) );
                break;
            case 20:
                _v.move(-std::abs(_v.x()), -std::abs(_v.y()) );
                break;
            case 28:
                _v.move( std::abs(_v.x()), -std::abs(_v.y()) );
                break;
            case 29:
            case 30:
            case 31:
            case 0:
            case 1:
            case 2:
            case 3:
                _v.move( std::abs(_v.x()), _v.y());
                break;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                _v.move( _v.x(), std::abs(_v.y()) );
                break;
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
                _v.move(-std::abs(_v.x()), _v.y());
                break;
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
                _v.move(_v.x(), -std::abs(_v.y()));
                break;
            default:
                assert(0 && "Illegal angle");
            }
        }
    }
    
#else
        // Check for bricks
    if(contains(bricks.bounding(), nv))
    {
        bool hit = false;
        for(auto& e : bricks.array())
        {
            if(!e.alive()) { continue; }
            if(contains(e.hitRect(), nv))
            {
                e.hit();
                auto br = e.hitRect();

#if 0
                auto xd = std::abs(nv.x() - br.center().x());
                auto yd = std::abs(nv.y() - br.center().y()) * (Bricks::Brick::WIDTH / (float)Bricks::Brick::HEIGHT);
                printf("HIT Brick %f,%f => %c\n", xd, yd, (xd <= yd) ? 'Y' : 'X');
                if(xd <= yd)
                {
                    _v.move(_v.x(), -_v.y());
                }
                else
                {
                    _v.move(-_v.x(), _v.y());
                }
#else
                auto angle = table_atan2(nv.y() - br.center().y(), nv.x() - br.center().x());
                printf("hitBrick %d\n", angle);
                switch(angle)
                {
                case -1:
                    _v.move(-_v.x(), -v.y());
                    break;
                case 4:
                    _v.move( std::abs(_v.x()),  std::abs(_v.y()) );
                    break;
                case 12:
                    _v.move(-std::abs(_v.x()),  std::abs(_v.y()) );
                    break;
                case 20:
                    _v.move(-std::abs(_v.x()), -std::abs(_v.y()) );
                    break;
                case 28:
                    _v.move( std::abs(_v.x()), -std::abs(_v.y()) );
                    break;
                case 29:
                case 30:
                case 31:
                case 0:
                case 1:
                case 2:
                case 3:
                    _v.move( std::abs(_v.x()), _v.y());
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                    _v.move( _v.x(), std::abs(_v.y()) );
                    break;
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                    _v.move(-std::abs(_v.x()), _v.y());
                    break;
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                    _v.move(_v.x(), -std::abs(_v.y()));
                    break;
                default:
                    assert(0 && "Illegal angle");
                }
#endif

                break;
            }
        }
    }
#endif
    nv = ov + _v;
}
/*
2\6,5,4,3,2,1,0,1,2,3,4,5,6/2
1                           1
0                           0
1                           1
2/6,5,4,3,2,1,0,1,2,3,4,5,6\2


0,2
1,2
3,2
4,2


6,2 X/Y
6,1 X
6,0 X


 */


// ----------------------------------------------------------------------------
// Ball_3
// ----------------------------------------------------------------------------
void Ball_3::update(Paddle& paddle, Bricks& bricks)
{
    if(ready()) { return; }

    _prev = _center;
    
    int cnt = 0;
    Vec2 ov = _center;
    Vec2 nv = _center + _v * velocity();

    //    PRINTF("----------------[%d]\n", id());
    assert(contains(FIELD_HIT_RECT, _center) && "OUT OF FIELD");
    
    while(_update(ov, nv, paddle, bricks) )
    {
        ++cnt;
        //        if(cnt > 1) { PRINTF("id:%d cnt:%d\n", _id, cnt); }
    }
    _center = nv;
    
    _history.push_back(_center);
}

bool Ball_3::_update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks)
{
    Vec2 iov = ov;
    Vec2 inv = nv;
    Vec2 cp, rp;
    auto pr = paddle.hitRect();
    Vec2 pv[4][2] =
    {
        { Vec2(pr.leftTop()),     Vec2(pr.rightTop()) },    // 0:upper
        { Vec2(pr.rightTop()),    Vec2(pr.rightBottom()) }, // 1:right
        { Vec2(pr.rightBottom()), Vec2(pr.leftBottom()) },  // 2:bottom
        { Vec2(pr.leftBottom()),  Vec2(pr.leftTop()) },     // 3:left
    };

    // PRINTF("update (%d,%d)-(%d,%d) PL:(%d,%d)-(%d,%d)\n",
    //        (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y(),
    //        pr.left(), pr.top(), pr.right(), pr.bottom());

    // Check for game field.
    for(size_t i = 0; i < goblib::size(FIELD_VECTOR); ++i)
    {
        // PRINTF("    FIELD[%d] (%d,%d) - %d,%d) O:%d/%d N:%d/%d\n",
        //        i, (int)FIELD_VECTOR[i][0].x(), (int)FIELD_VECTOR[i][0].y(),
        //        (int)FIELD_VECTOR[i][1].x(), (int)FIELD_VECTOR[i][1].y(),
        //        isLeft(ov, FIELD_VECTOR[i][0], FIELD_VECTOR[i][1]),
        //        isRight(ov, FIELD_VECTOR[i][0], FIELD_VECTOR[i][1]),
        //        isLeft (nv, FIELD_VECTOR[i][0], FIELD_VECTOR[i][1]),
        //        isRight (nv, FIELD_VECTOR[i][0], FIELD_VECTOR[i][1]) );

        if((//isRight(ov, FIELD_VECTOR[i][0], FIELD_VECTOR[i][1]) ||
            isLeft (nv, FIELD_VECTOR[i][0], FIELD_VECTOR[i][1]) ) &&
           intersection(ov, nv, FIELD_VECTOR[i][0],  FIELD_VECTOR[i][1],  &rp, &cp) )
        {
            auto rv = rp - cp;
            // PRINTF("rv(%f,%f) ",rv.x(), rv.y() ); 

            if(std::isgreater(rv.lengthSq(), 0.0f))
            {
                _v = rv.normalizeV();
                ov = cp;
                nv = rp;
            }
            // PRINTF("v:%f,%f cp(%d,%d) - rp(%d,%d)\n",
            //        _v.x(), _v.y(), (int)cp.x(), (int)cp.y(), (int)nv.x(), (int)nv.y());
            return true;
        }
    }
    bool oout = !contains(FIELD_HIT_RECT, ov);
    bool nout = !contains(FIELD_HIT_RECT, nv);
    bool oin = contains(pr, ov);
    bool nin = contains(pr, nv);
    float ydir = goblib::math::sign(nv.y() - ov.y()); // 1.0f:to bottom, -1.0f:to top 0:horizon
    Vec2 pmove = paddle.vector();

#if 1
    assert(!(oout && nout) && "Out of field");

    // Check for paddle.
    // [Collision F] shift to top or bottom
    if(oin && nout)
    {
        //        Vec2 off(0.0f, ((ydir >= 0.0f) ? pr.top() : pr.bottom()) - ov.y() + (ydir == 0.0f) ? -1.0f : -ydir);
        Vec2 off(0.0f, (ydir >= 0.0f) ? pr.top() - ov.y() - 1.0f : pr.bottom() - ov.y() + 1.0f );
        PRINTF(">>SHIFT F (%f,%f)\n", off.x(), off.y());
        ov += off;
        nv += off;
        return true;
    }

    // [Collision C] ov.nv shift by paddle.
    if((oin && nin) && pmove.x() != 0.0f)
    {
        PRINTF_S(">>SHIFT by PADDLE\n");
        ov += pmove;
        nv += pmove;

        // [F] Out of field to out of field?
        PRINTF("  contains %d:%d\n", contains(FIELD_HIT_RECT, ov),contains(FIELD_HIT_RECT, nv));
        if(!contains(FIELD_HIT_RECT, ov) && !contains(FIELD_HIT_RECT, nv))
        {
            PRINTF_S(">>SHIFT F2\n");
            Vec2 off(0.0f, (ydir >= 0.0f) ? pr.top() - ov.y() - 1.0f : pr.bottom() - ov.y() + 1.0f );
            ov = iov + off;
            nv = inv + off;
        }
        return true;
    }

    // [Coollison C,F] ov,nv shift to ov out of paddle.
    if((oin && nin))
    {
        PRINTF_S(">>SHIFT\n");
        Vec2 mv = (ov - nv).normalizeV();
        Vec2 ov2 = ov + mv * 320.0f;;

        for(int_fast8_t i = 0 ; i < 4; ++i)
        {
            if(isRight(ov, pv[i][0], pv[i][1]) && intersection(ov, ov2, pv[i][0], pv[i][1], &rp, &cp))
            {
                PRINTF("ov2:(%d,%d) cp[%d](%d,%d) nv(%d,%d)",
                       (int)ov2.x(), (int)ov2.y(),
                       i, (int)cp.x(), (int)cp.y(),
                       (int)nv.x(), (int)nv.y());

                auto offset = (cp - ov) + mv * 0.01f;
                ov += offset;
                nv += offset;

                PRINTF("update (%d,%d)-(%d,%d) PL:(%d,%d)-(%d,%d)\n",
                       (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y(),
                       pr.left(), pr.top(), pr.right(), pr.bottom());
                break;
            }
        }
    }

    // [Collision A,B]
    for(int i = 0 ; i < 4; ++i)
    {
        if(isLeft(ov, pv[i][0], pv[i][1]) && intersection(ov, nv, pv[i][0], pv[i][1], &rp, &cp))
        {
            PRINTF("PADDLE (%d,%d)-(%d,%d) ", (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y());
            
            _v = (rp - cp).normalizeV();
            ov = cp + _v * 0.001f;
            nv = rp;

            PRINTF("v:%f,%f cp(%d,%d) - rp(%d,%d)\n", _v.x(), _v.y(), (int)cp.x(), (int)cp.y(), (int)nv.x(), (int)nv.y());
            return true;
        }
    }
#endif

    
    return false;

#if 0
FBOTTOM (315,235)-(4,235)
        
#endif
}
