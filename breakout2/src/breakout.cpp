/*!
  Breakout
  @brief Simple breakout game
*/
#include <LovyanGFX.hpp>
#include "breakout.hpp"
#include "app.hpp"
#include <gob_math.hpp>
#include <gob_utility.hpp>
#include <gob_template_helper.hpp>
#include <gob_m5s_random.hpp>
#include <gob_suffix.hpp>
using goblib::suffix::operator"" _u8;
#include <limits>
#include <algorithm>
#include <vector>

namespace
{
// Is Rect(int16) contains Vec2(float)
bool contains(const Rect2& r, const Vec2& pos)
{
    return std::isgreaterequal(pos.x(), r.left()) && std::islessequal(pos.x(), r.right()) &&
            std::isgreaterequal(pos.y(), r.top()) && std::islessequal(pos.y(), r.bottom());
}

// For when the ball hit the paddle.
constexpr std::int16_t PADDLE_HIT_HOR = 6;
constexpr std::int16_t PADDLE_HIT_VER = 1;
constexpr std::int16_t PADDLE_HIT_WIDTH = ((Paddle::WIDTH + Ball::RADIUS * 2) / 2) / PADDLE_HIT_HOR;
constexpr std::int16_t PADDLE_HIT_HEIGHT = ((Paddle::HEIGHT + Ball::RADIUS * 2) / 2) / PADDLE_HIT_VER;
constexpr float refPaddleTable[PADDLE_HIT_HOR * PADDLE_HIT_VER] =
{
    goblib::math::deg2rad(11.25f * 7),
    goblib::math::deg2rad(11.25f * 6),
    goblib::math::deg2rad(11.25f * 5),
    goblib::math::deg2rad(11.25f * 4),
    goblib::math::deg2rad(11.25f * 3),
    goblib::math::deg2rad(11.25f * 2),
};
// For when the ball hit the brick.
constexpr std::int16_t BRICK_HIT_HOR = 5;
constexpr std::int16_t BRICK_HIT_VER = 6;
constexpr std::int16_t BRICK_HIT_WIDTH = ((Bricks::Brick::WIDTH + Ball::RADIUS * 2) / 2) / BRICK_HIT_HOR;
constexpr std::int16_t BRICK_HIT_HEIGHT = ((Bricks::Brick::HEIGHT + Ball::RADIUS * 2) / 2) / BRICK_HIT_VER;
constexpr std::uint8_t refBrickTable[BRICK_HIT_HOR * BRICK_HIT_VER] =
{
    0x03, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x00, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x00, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x00,
};
//
}

// ----------------------------------------------------------------------------
// Paddle
// ----------------------------------------------------------------------------
goblib::lgfx::CellRect Paddle::_cell(64,8, WIDTH, HEIGHT);

Paddle::Paddle(goblib::lgfx::CellSprite& image)
        : _rect(INITIAL_LEFT, INITIAL_TOP, WIDTH, HEIGHT)
        , _hitRect(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS, _rect.width() + Ball::RADIUS * 2, _rect.height() + Ball::RADIUS * 2)
        , _prev(_rect.leftTop())
        , _sprite(&image)
{
    PRINTF("PADDLE: r(%d,%d)[%d,%d] hr(%d,%d)[%d,%d]\n",
           _rect.left(), _rect.top(), _rect.width(), _rect.height(),
           _hitRect.left(), _hitRect.top(), _hitRect.width(), _hitRect.height());
}

void Paddle::rewind()
{
    _rect.move(INITIAL_LEFT, INITIAL_TOP);
    _hitRect.move(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS);
}

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
    _sprite->pushCell(s, _cell, _rect.left(), _rect.top() - yoffset, 0);

#ifdef DEBUG_RENDER
    s->drawRect(_hitRect.left(), _hitRect.top() - yoffset, _hitRect.width(), _hitRect.height(), 0xF800);
#endif
}

// ----------------------------------------------------------------------------
// Bricks
// ----------------------------------------------------------------------------
goblib::lgfx::CellSprite* Bricks::Brick::_sprite;
const std::vector<std::vector<goblib::lgfx::CellRect>> Bricks::Brick::_cells = 
{
    // None
    {
        goblib::lgfx::CellRect(),
    },
    // Hard brick
    {
        goblib::lgfx::CellRect(WIDTH * 0, HEIGHT * 2, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 1, HEIGHT * 2, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 2, HEIGHT * 2, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 3, HEIGHT * 2, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 4, HEIGHT * 2, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 5, HEIGHT * 2, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 1, HEIGHT * 0, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 2, HEIGHT * 0, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 0, HEIGHT * 1, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 1, HEIGHT * 1, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 2, HEIGHT * 1, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 3, HEIGHT * 1, WIDTH, HEIGHT),
    },
    {
        goblib::lgfx::CellRect(WIDTH * 0, HEIGHT * 0, WIDTH, HEIGHT),
    },
    // Unbreakable
    {
        goblib::lgfx::CellRect(WIDTH * 0, HEIGHT * 3, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 1, HEIGHT * 3, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 2, HEIGHT * 3, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 3, HEIGHT * 3, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 4, HEIGHT * 3, WIDTH, HEIGHT),
        goblib::lgfx::CellRect(WIDTH * 5, HEIGHT * 3, WIDTH, HEIGHT),
    },
};

using Seq = goblib::graph::Sequence;
goblib::graph::AnimationSequencer Bricks::Brick::_sequencers[2] =
{
    {
        Seq(Seq::Draw, 0, 1_u8),
    },
    {
        Seq(Seq::Draw, 1, 3_u8),
        Seq(Seq::Draw, 2, 3_u8),
        Seq(Seq::Draw, 3, 3_u8),
        Seq(Seq::Draw, 4, 3_u8),
        Seq(Seq::Draw, 5, 3_u8),
        Seq(Seq::Draw, 0, 1_u8),
    }
};

Bricks::Brick::Brick(std::int16_t x, std::int16_t y, std::uint8_t type)
        : _type(type)
        , _life(type != 0)
        , _rect(x, y, WIDTH, HEIGHT)
        , _hitRect(_rect.left() - Ball::RADIUS, _rect.top() - Ball::RADIUS, _rect.width() + Ball::RADIUS * 2, _rect.height() + Ball::RADIUS * 2)
        , _animation(_sequencers[type == Unbreakable || type == 1])
{
    if(type == 1) { _life = 2; }
}

void Bricks::Brick::hit()
{
    if(_animation.isFinish()) {    _animation.reset(); }
    if(_life > 0) { --_life; }
}

void Bricks::Brick::pump()
{
    _animation.pump();
}

void Bricks::Brick::render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
    if(alive())
    {
        _sprite->pushCell(s, _cells[_type][_animation.cell()], _rect.left(), _rect.top() - yoffset);
        
#ifdef DEBUG_RENDER
        s->drawRect(_hitRect.left(), _hitRect.top() - yoffset, _hitRect.width(), _hitRect.height(), 0xF800);
#endif
    }
}

bool Bricks::isClear() const
{
    return std::all_of(_bricks.begin(), _bricks.end(),
                       [](const Brick& e) { return e.type() == Brick::Unbreakable || !e.alive(); });
}

Rect2 Bricks::bounding() const
{
    std::int16_t left(FIELD_HIT_RECT.right()), top(FIELD_HIT_RECT.bottom()),
                 right(FIELD_HIT_RECT.left()), bottom(FIELD_HIT_RECT.top());

    for(auto& e : _bricks)
    {
        if(!e.alive()) { continue; }
        left = std::min(left, e.hitRect().left());
        right = std::max(right, e.hitRect().right());
        top = std::min(top, e.hitRect().top());
        bottom = std::max(bottom, e.hitRect().bottom());
    }
    return Rect2(left, top, right - left + 1, bottom - top + 1);
}

void Bricks::pump()
{
    for(auto& e : _bricks)
    {
        if(!e.alive()) { continue; }
        e.pump();
    }    
}

void Bricks::render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset)
{
    for(auto& e : _bricks)
    {
        e.render(s, yoffset);
    }

#ifdef DEBUG_RENDER
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
goblib::lgfx::CellRect Ball::_cell = goblib::lgfx::CellRect(64,0,8,8);
goblib::lgfx::CellSprite* Ball::_sprite;

void Ball::launch()
{
    if(_launch) { return; }
    _launch = true;

    if(_v.length() == 0.0f)
    {
        goblib::m5s::arduino_engine re;
        std::uniform_int_distribution<> distx(0, goblib::size(refPaddleTable) - 1);
        auto i = distx(re);
        PRINTF("[%d] %f:%f\n", i, refPaddleTable[i], std::cos(refPaddleTable[i]));

        _v.move(std::cos(refPaddleTable[ i ]), -1.0f);
        _v.normalize();
    }
    PRINTF("BALL:launch %d,%f,%f\n", _velocity, _v.x(), _v.y());
}

void Ball::render(goblib::lgfx::GSprite* s, const std::int_fast16_t yoffset)
{
    _sprite->pushCell(s, _cell, _center.x() - 4, _center.y() - 4 - yoffset, 0);
    
#ifdef DEBUG_RENDER
    if(!_history.empty())
    {
        for(size_t i = 0; i < _history.size() - 1; ++i)
        {
            s->drawLine(_history[i].x(),   _history[i].y() - yoffset,
                        _history[i+1].x(), _history[i+1].y() - yoffset, _color ^ 0xFFFF);
        }
    }
#endif
}

void Ball::update(Paddle& paddle, Bricks& bricks)
{
    if(ready()) { return; }

    PRINTF_S("----\n");
    _prev = _center;

    if(!FIELD_HIT_RECT.contains(_center.x(), _center.y()))
    {
        PRINTF("(%d,%d)-(%d,%d) %f,%f\n",
               FIELD_HIT_RECT.left(), FIELD_HIT_RECT.top(), FIELD_HIT_RECT.right(), FIELD_HIT_RECT.bottom(),
               _center.x(), _center.y());
        assert(0);
    }
    
    int cnt = _velocity;
    Vec2 ov;
    Vec2 nv = _center;
    while(cnt--)
    {
        ov = nv;
        nv = ov + _v;
        _update(ov, nv, paddle, bricks);
        PRINTF("  _update (%d,%d) - (%d,%d)\n", (int)ov.x(), (int)ov.y(), (int)nv.x(), (int)nv.y());
    }

    _center = nv;
    ++_framesNotHittingPaddle;
    updateSpeed();
    
    PRINTF("UPDATE (%d,%d) - (%d,%d)\n", (int)_prev.x(), (int)_prev.y(), (int)nv.x(), (int)nv.y());

#ifdef DEBUG_RENDER
    _history.push_back(_center);
#endif
}

// move 1 step.
void Ball::_update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks)
{
    // Check for paddle
    auto pr = paddle.hitRect();
    if(contains(pr, nv))
    {
        _framesNotHittingPaddle = 0;
#if 0
        //Using atan2
        int xd = nv.x() - pr.center().x();
        int yd = nv.y() - pr.center().y();
        if(yd == 0) { yd = -1; }
        auto radian = std::atan2(yd, xd);
        _v.move(std::cos(radian), std::sin(radian));

        PRINTF("HIT PADDLE radian:%f/%f v:(%f,%f) %f\n",
               radian, goblib::math::rad2deg(radian), _v.x(), _v.y(), _v.length());

#else
        auto xd = nv.x() - pr.center().x();
        auto yd = nv.y() - pr.center().y();
        int xi = std::abs(xd) / PADDLE_HIT_WIDTH;
        int yi = std::abs(yd) / PADDLE_HIT_HEIGHT;

        float radian = refPaddleTable[ yi * PADDLE_HIT_HOR + xi];
        if(xd < 0) { radian = goblib::math::deg2rad(180.0f) - radian; }
        if(yd < 0) { radian = goblib::math::deg2rad(360.0f) - radian; }
        radian = goblib::math::wrapRad(radian);

        _v.move(std::cos(radian), std::sin(radian));

        PRINTF("HIT PADDLE : [%f,%f] <%d,%d> %f/%f v(%f,%f) %f\n",
               xd, yd, xi, yi,
               radian, goblib::math::rad2deg(radian),
               _v.x(), _v.y(), _v.length());

        assert(yi * PADDLE_HIT_HOR + xi < goblib::size(refPaddleTable) && "Illegal index");
#endif
        onHitPaddle();
    }

    // Check for bricks
    if(contains(bricks.bounding(), nv))
    {
        std::vector<Bricks::Brick*> hit;
        for(auto& e : bricks.vector())
        {
            if(!e.alive()) { continue; }
            if(contains(e.hitRect(), nv))
            {
                hit.push_back(&e);
            }
        }
        if(!hit.empty())
        {
            PRINTF("hit bricks %zu ball(%f,%f)\n", hit.size(), nv.x(), nv.y());
            Rect2 br;
            for(auto& e : hit)
            {
                br |= e->hitRect();
                e->hit();
                onHitBrick(*e);
                PRINTF("  hit brick %zu\n", (e - bricks.vector().data()));
            }
            auto xd = nv.x() - br.center().x();
            auto yd = nv.y() - br.center().y();
            int xi = std::abs(xd) / BRICK_HIT_WIDTH;
            int yi = std::abs(yd) / BRICK_HIT_HEIGHT;
            auto behavior = refBrickTable[ yi * BRICK_HIT_HOR + xi ];

            if(behavior == 0)   { _v.move(goblib::math::sign(xd) * std::abs(_v.x()), goblib::math::sign(yd) * std::abs(_v.y()) ); }
            if(behavior & 0x01) { _v.move(-_v.x(), _v.y()); }
            if(behavior & 0x02) { _v.move(_v.x(), -_v.y()); }

            if(_framesNotHittingPaddle >= NUMBER_OF_FRAMES_TO_FORCE_ANGLE_CHANGE)
            {
                _framesNotHittingPaddle = 0;
                auto radian = std::atan2(std::abs(_v.y()), std::abs(_v.x()));
                PRINTF(" force change angle : v(%f,%f) %f => ", _v.x(), _v.y(), std::atan2(_v.y(), _v.x()));

                if(radian > goblib::math::deg2rad(45.0f)) { radian -= goblib::math::deg2rad(11.25f); }
                else { radian += goblib::math::deg2rad(11.25f); }
                if(std::isless(_v.x(), 0.0f)) { radian = goblib::math::deg2rad(180.0f) - radian; }
                if(std::isless(_v.y(), 0.0f)) { radian = goblib::math::deg2rad(360.0f) - radian; }
                radian = goblib::math::wrapRad(radian);
                _v.move(std::cos(radian), std::sin(radian));
                PRINTF(" v(%f,%f) %f\n", _v.x(), _v.y(), radian);
            }
            PRINTF("hit bricks %zu (%f,%f) [%d,%d] beh:%02x v:%f,%f) ov(%f,%f) br(%d,%d)-(%d,%d) <%d,%d> nv(%f,%f)\n",
                   hit.size(), xd, yd, xi, yi,
                   behavior, _v.x(), _v.y(), ov.x(), ov.y(),
                   br.left(), br.top(), br.right(), br.bottom(),br.center().x(), br.center().y(),
                   nv.x(), nv.y());
            assert(yi * BRICK_HIT_HOR + xi < goblib::size(refBrickTable) && "Illegal index");
        }
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
    
    nv = ov + _v;
}

void Ball::onHitBrick(const Bricks::Brick& b)
{
    ++_hitCount;
    if(b.type() != Bricks::Brick::Unbreakable && !b.alive())
    {
        Breakout::instance().addScore(100);
    }
}

void Ball::onHitPaddle()
{
}

void Ball::onHitWall()
{
    ++_hitCount;
}
