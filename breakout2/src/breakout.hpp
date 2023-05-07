/*!
  Breakout
  @brief Simple breakout game
*/
#ifndef BREAKOUT_HPP
#define BREAKOUT_HPP

#include "debug.hpp"
#include "typedef.hpp"
#include <lgfx/gob_lgfx.hpp>
#include <gob_ring_buffer.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <lgfx/gob_lgfx_animated_sprite.hpp>
#include <initializer_list>
#include <vector>


// Field of playing(for render)
constexpr std::int16_t FIELD_WIDTH = 16*13;
constexpr Rect2 FIELD_RECT((320-FIELD_WIDTH)/2,16, FIELD_WIDTH, 240-16);

// ----------------------------------------------------------------------------
class Paddle
{
  public:
    constexpr static std::int16_t WIDTH = 32;
    constexpr static std::int16_t HEIGHT = 8;
    constexpr static std::int16_t INITIAL_LEFT = 320/2 - WIDTH/2;
    constexpr static std::int16_t INITIAL_TOP = FIELD_RECT.bottom() - (HEIGHT*2);

    Paddle(goblib::lgfx::CellSprite& image);

    const Rect2& rect() const { return _rect; }
    const Rect2& hitRect() const { return _hitRect; }

    void rewind();
    void offset(std::int16_t ox, std::int16_t oy);
    void render(LGFX_Sprite* s, std::int_fast16_t yoffset);
    
  private:
    Rect2 _rect, _hitRect;
    Pos2 _prev;
    goblib::lgfx::CellSprite* _sprite;

    static goblib::lgfx::CellRect _cell;
};

// ----------------------------------------------------------------------------
class Bricks
{
  public:
    //
    class Brick
    {
      public:
        constexpr static std::int16_t WIDTH = 16;
        constexpr static std::int16_t HEIGHT = 8;

        enum Type : std::uint8_t { None/*0*/, Clr1, Clr2, Clr3, Clr4, Clr5, Clr6, Clr7, Clr8, Unbreakable/*9*/ };
        
        Brick(std::int16_t x, std::int16_t y, std::uint8_t type);
            
        const Rect2& rect() const { return _rect; }
        const Rect2& hitRect() const { return _hitRect; }
        std::uint8_t type() const { return _type; }
        bool alive() const { return _type == Unbreakable || _life > 0; }
        void hit();
        
        void pump();
        void render(LGFX_Sprite* s, std::int_fast16_t yoffset);

      private:
        std::uint8_t _type;
        std::int8_t _life;
        Rect2 _rect, _hitRect;
        goblib::graph::AnimationSequencer _animation;
        
        static goblib::lgfx::CellSprite* _sprite;
        static const std::vector<std::vector<goblib::lgfx::CellRect>> _cells;
        static goblib::graph::AnimationSequencer _sequencers[2];

        friend class Bricks;
    };

    constexpr static std::int16_t LEFT = FIELD_RECT.left();
    constexpr static std::int16_t TOP = FIELD_RECT.top();
    constexpr static std::int16_t NUM_OF_HORIZONTAL = 13;

    Bricks(goblib::lgfx::CellSprite& image) : _bricks(), _sprite(&image)
    {
        Brick::_sprite = _sprite;
    }
    
    template <class InputIter> void set(InputIter first, InputIter last);
    
    bool isClear() const;
    std::vector<Brick>& vector() { return _bricks; }
    Rect2 bounding() const;

    void pump();
    void render(LGFX_Sprite* s, std::int_fast16_t yoffset);

  private:    
    std::vector<Brick> _bricks;
    goblib::lgfx::CellSprite* _sprite;
};

template <class InputIter> void Bricks::set(InputIter first, InputIter last)
{
    std::int16_t x = 0;
    std::int16_t y = 0;
    _bricks.clear();
    while(first != last)
    {
        if(*first)
        {
            _bricks.emplace_back(LEFT + Brick::WIDTH * x, TOP + Brick::HEIGHT * y, *first);
        }
        if(++x >= NUM_OF_HORIZONTAL)
        {
            x = 0;
            ++y;
        }
        ++first;
    }
}

// ----------------------------------------------------------------------------
// Ball
class Ball
{
  public:
    constexpr static std::int16_t RADIUS = 2;

    Ball(Vec2::pos_type ix, Vec2::pos_type iy)
            : _center(ix, iy)
            , _prev(_center)
            , _v()
            , _velocity(4.0f)
            , _launch(false)
            , _hitCount(0)
            , _framesNotHittingPaddle(0)
#ifdef DEBUG_RENDER
            , _history()
#endif
    {}

    Ball(Vec2::pos_type ix, Vec2::pos_type iy, const Vec2& mv, std::int16_t spd)
            : Ball(ix, iy)
    {
        assert(spd > 0);
        _v = mv.normalizeV();
        _velocity = spd;
    }
    
    bool ready() const { return _launch == false; }
    Vec2 center() const { return _center; }
    std::int16_t velocity() const { return _velocity; }
    Vec2 vector() const { return _v; }
    std::uint32_t framesNohit() const { return _framesNotHittingPaddle; }
    
    void launch();
    virtual void update(Paddle& paddle, Bricks& blocks);
    virtual void render(LGFX_Sprite* s, std::int_fast16_t yoffset);

    static goblib::lgfx::CellSprite* _sprite;
    
  protected:
    void _update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks);
    void onHitBrick(const Bricks::Brick& b);
    void onHitPaddle();
    void onHitWall();
    void updateSpeed() { if(_velocity < VELOCITY_MAX && _hitCount >= VELOCITY_UP_COUNT) { _hitCount = 0; ++_velocity; } }

  private:
    Vec2 _center, _prev;
    Vec2 _v; // direction vector(normalized)
    std::int16_t _velocity; // moving speed.
    bool _launch;
    std::uint32_t _hitCount;
    std::uint32_t _framesNotHittingPaddle;

#ifdef DEBUG_RENDER
    goblib::RingBuffer<Vec2, 16> _history;
#endif
    constexpr static std::uint32_t NUMBER_OF_FRAMES_TO_FORCE_ANGLE_CHANGE = 30 * 10;
    constexpr static std::uint32_t VELOCITY_UP_COUNT = 8;
    constexpr static std::uint16_t VELOCITY_MAX = 10;

    static goblib::lgfx::CellRect _cell;
};

// Field of playing(for collision)
constexpr Rect2 FIELD_HIT_RECT(FIELD_RECT.left() + Ball::RADIUS, FIELD_RECT.top() + Ball::RADIUS,
                               FIELD_RECT.width() - Ball::RADIUS * 2, FIELD_RECT.height() - Ball::RADIUS * 2);

#endif
