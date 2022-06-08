#ifndef BREAKOUT_DEFINITION_HPP
#define BREAKOUT_DEFINITION_HPP

#include <cstdint>

enum class BGM : std::uint8_t { Bgm1, StartGame, ClearGame, Miss, Max };
enum class SFX : std::uint8_t { HitBrick, HitPaddle, Max };

#endif
