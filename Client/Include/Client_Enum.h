#pragma once

enum class PROJ_TYPE
{
    LINEAR,
    ORBIT,
    HOMING,
    TYPE_END
};

enum class BOSSPHASE1
{
    BOSSAWAKE = 0,
    BULLETATTACK,
    CHAINATTACK,
    BOSSDEAD,
    BOSSIDLE,
    BOSSMOVE,
    BOSSGROGGRY,
    BOSS_END
};

enum FOODQUALITY
{
    GOOD,
    BAD,
    NO_FOOD
};

// 타이머용 enum
enum TIMER
{
    TIMER_SOUND_PL_FOOTSTEP,  // 달리기 효과음 재생용
    TIMER_SOUND_PL_ACTION,    // 액션 효과음 재생용 (채집에 사용중)

    TIMER_END
};
