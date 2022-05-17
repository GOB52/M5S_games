
#ifndef BREAKOUT_HPP
#define BREAKOUT_HPP

#include "debug.hpp"
#include "typedef.hpp"
#include <lgfx/gob_lgfx.hpp>
#include <gob_ring_buffer.hpp>

#include <initializer_list>
#include <vector>

// Field of playing(for render)
#ifdef BEHAVIOR_TEST
constexpr Rect2 FIELD_RECT((320-200)/2,(240-200)/2, 200, 200);
#else
constexpr std::int16_t FIELD_WIDTH = 16*13;
constexpr Rect2 FIELD_RECT((320-FIELD_WIDTH)/2,(240-200)/2, FIELD_WIDTH, 200);
#endif

// ----------------------------------------------------------------------------
class Paddle
{
  public:
    constexpr static std::int16_t WIDTH = 32;
    constexpr static std::int16_t HEIGHT = 8;
    constexpr static std::int16_t INITIAL_LEFT = 320/2 - WIDTH/2;
#ifdef BEHAVIOR_TEST
    constexpr static std::int16_t INITIAL_TOP = FIELD_RECT.bottom() - 80;
#else
    //    constexpr static std::int16_t INITIAL_TOP = FIELD_RECT.bottom() - 16;
    constexpr static std::int16_t INITIAL_TOP = FIELD_RECT.bottom() - (HEIGHT*4);
#endif

    Paddle();

    const Rect2& rect() const { return _rect; }
    const Rect2& hitRect() const { return _hitRect; }
    const Vec2 vector() const { return Vec2(_rect.leftTop() - _prev); }

    void rewind();
#ifdef BEHAVIOR_TEST
    void move(std::int16_t x, std::int16_t y);
#endif
    void offset(std::int16_t ox, std::int16_t oy);

    void render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset);
    
  private:
    Rect2 _rect, _hitRect;
    Pos2 _prev;
};

// ----------------------------------------------------------------------------
class Bricks
{
  public:
    class Brick
    {
      public:
        constexpr static std::int16_t WIDTH = 16;
        constexpr static std::int16_t HEIGHT = 8;

        enum Type : std::uint8_t { None, Clr1, Clr2, Clr3, Clr4, Clr5, Clr6, Clr7, Clr8, Unbreakable };
        
        Brick(std::int16_t x, std::int16_t y, std::uint8_t type);
            
        const Rect2& rect() const { return _rect; }
        const Rect2& hitRect() const { return _hitRect; }

        bool alive() const { return _type == Unbreakable || _life > 0; }
        void hit() { if(_life > 0) { --_life; } }
        
        void render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset);

      private:
        std::uint8_t _type;
        std::int8_t _life;
        Rect2 _rect, _hitRect;
    };

    constexpr static std::int16_t LEFT = FIELD_RECT.left();
    constexpr static std::int16_t TOP = FIELD_RECT.top();
    constexpr static std::int16_t NUM_OF_HORIZONTAL = 13;
    
    Bricks(std::initializer_list<Brick> init) : _array(init.begin(), init.end()) {}
    Bricks(std::initializer_list<std::uint8_t> init);
    
    bool isClear() const;
    std::vector<Brick>& array() { return _array; }
    Rect2 bounding() const;

    Brick* get(std::int16_t x, std::int16_t y);

    
    void render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset);

  private:    
    std::vector<Brick> _array;
};

// ----------------------------------------------------------------------------
// Ball base
class Ball
{
  public:
#ifdef BEHAVIOR_TEST
    constexpr static std::int16_t RADIUS = 8;
#else
    constexpr static std::int16_t RADIUS = 2;
#endif
    
    Ball(Vec2::pos_type ix, Vec2::pos_type iy, std::uint16_t clr)
            : _center(ix, iy)
            , _prev(_center)
            , _v()
            , _color(clr)
            , _id(0)
            , _velocity(8.0f)
            , _launch(false)
    {
        static int id = 0;
        _id = id++;
        _history.push_back(_center);
    }
    Ball(Vec2::pos_type ix, Vec2::pos_type iy, const Vec2& mv, float spd, std::uint16_t clr)
            : Ball(ix, iy, clr)
    {
        _v = mv.normalizeV();
        _velocity = spd;
    }
    virtual ~Ball(){}
    
    bool ready() const { return _launch == false; }
    Rect2 rect() const { return Rect2(_center.x() - RADIUS, _center.y() - RADIUS, RADIUS * 2, RADIUS * 2); }
    Vec2 center() const { return _center; }
    float velocity() const { return _velocity; }
    Vec2 vector() const { return _v; }
    int id() const { return _id; }
    
    void launch();
    virtual void update(Paddle& paddle, Bricks& blocks) = 0;
    virtual void render(goblib::lgfx::GSprite* s, std::int_fast16_t yoffset);

  protected:
    void onHitBrick() { _velocity += 1.0f; }
    void onHitPaddle() { }
    void onHitWall() { _velocity += 1.0f; }
    
  protected:
    Vec2 _center, _prev;
    Vec2 _v; // direction vector(normalized)
    std::uint16_t _color;
    goblib::RingBuffer<Vec2, 16> _history;
    int _id;    

  private:
    float _velocity; // moving speed.
    bool _launch;

};

// Using algorithm 1
class Ball_1 : public Ball
{
  public:
    Ball_1(Vec2::pos_type ix, Vec2::pos_type iy, std::uint16_t clr) : Ball(ix, iy, clr) {}
    Ball_1(Vec2::pos_type ix, Vec2::pos_type iy, const Vec2& mv, float spd, std::uint16_t clr) : Ball(ix, iy, mv, spd, clr) {}

    virtual void update(Paddle& paddle, Bricks& bricks) override;

  private:
    void _update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks);

};

// Using algorithm 2
class Ball_2 : public Ball
{
  public:
    Ball_2(Vec2::pos_type ix, Vec2::pos_type iy, std::uint16_t clr) : Ball(ix, iy, clr) {}
    Ball_2(Vec2::pos_type ix, Vec2::pos_type iy, const Vec2& mv, float spd, std::uint16_t clr) : Ball(ix, iy, mv, spd, clr) {}

    virtual void update(Paddle& paddle, Bricks& bricks) override;

  private:
    void _update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks);
};

// Using algorithm 3
class Ball_3 : public Ball
{
  public:
    Ball_3(Vec2::pos_type ix, Vec2::pos_type iy, std::uint16_t clr) : Ball(ix, iy, clr) {}
    Ball_3(Vec2::pos_type ix, Vec2::pos_type iy, const Vec2& mv, float spd, std::uint16_t clr) : Ball(ix, iy, mv, spd, clr) {}
    virtual void update(Paddle& paddle, Bricks& bricks) override;

  private:
    bool _update(Vec2& ov, Vec2& nv, Paddle& paddle, Bricks& bricks);
};


// Field of playing(for collision)
constexpr Rect2 FIELD_HIT_RECT(FIELD_RECT.left() + Ball::RADIUS, FIELD_RECT.top() + Ball::RADIUS,
                               FIELD_RECT.width() - Ball::RADIUS * 2, FIELD_RECT.height() - Ball::RADIUS * 2);

#endif
