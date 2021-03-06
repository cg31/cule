#pragma once

#include <cule/config.hpp>
#include <cule/atari/controller.hpp>
#include <cule/atari/state.hpp>

namespace cule
{
namespace atari
{
namespace games
{
namespace frostbite
{

template<typename State>
CULE_ANNOTATION
void reset(State& s)
{
    s.m_reward   = 0;
    s.m_score    = 0;
    s.tiaFlags.clear(FLAG_ALE_TERMINAL);
    s.m_lives 	 = 4;
}

template<typename State>
CULE_ANNOTATION
void step(State& s)
{
    using cule::atari::games::getDecimalScore;
    using cule::atari::ram::read;

    // update the reward
    int score = getDecimalScore(s, 0xCA, 0xC9, 0xC8);
    int reward = score - s.m_score;
    s.m_reward = reward;
    s.m_score = score;

    // update terminal status
    // MGB: the maximum achievable life is 9. The system will actually let us set the byte to
    // higher values & properly decrement, but we do not gain lives beyond 9.
    int lives_byte = (ram::read(s, 0xCC) & 0xF);
    int flag  = ram::read(s, 0xF1) & 0x80;
    s.tiaFlags.template change<FLAG_ALE_TERMINAL>((lives_byte == 0) && (flag != 0));

    s.m_lives = lives_byte + 1;
}

CULE_ANNOTATION
bool isMinimal(const Action &a)
{
    switch (a)
    {
    case ACTION_NOOP:
    case ACTION_FIRE:
    case ACTION_UP:
    case ACTION_RIGHT:
    case ACTION_LEFT:
    case ACTION_DOWN:
    case ACTION_UPRIGHT:
    case ACTION_UPLEFT:
    case ACTION_DOWNRIGHT:
    case ACTION_DOWNLEFT:
    case ACTION_UPFIRE:
    case ACTION_RIGHTFIRE:
    case ACTION_LEFTFIRE:
    case ACTION_DOWNFIRE:
    case ACTION_UPRIGHTFIRE:
    case ACTION_UPLEFTFIRE:
    case ACTION_DOWNRIGHTFIRE:
    case ACTION_DOWNLEFTFIRE:
        return true;
    default:
        return false;
    }
}

template<typename State>
CULE_ANNOTATION
int32_t lives(State& s)
{
    return (cule::atari::ram::read(s.ram, 0xCC) & 0xF) + 1;
}

template<typename State>
CULE_ANNOTATION
void setTerminal(State& s)
{
    // update terminal status
    // MGB: the maximum achievable life is 9. The system will actually let us set the byte to
    // higher values & properly decrement, but we do not gain lives beyond 9.
    int lives_byte = lives(s) - 1;
    int flag  = cule::atari::ram::read(s.ram, 0xF1) & 0x80;
    s.tiaFlags.template change<FLAG_ALE_TERMINAL>((lives_byte == 0) && (flag != 0));
}

template<typename State>
CULE_ANNOTATION
int32_t score(State& s)
{
    return cule::atari::games::getDecimalScore(s, 0xCA, 0xC9, 0xC8);
}

template<typename State>
CULE_ANNOTATION
int32_t reward(State& s)
{
    return score(s) - s.score;
}

} // end namespace frostbite
} // end namespace games
} // end namespace atari
} // end namespace cule

