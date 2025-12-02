#pragma once

enum class PROJ_TYPE { LINEAR, ORBIT, HOMING, TYPE_END };

// 심심해서 이렇게 만들어봄
namespace BossStateType
{
    enum Phase2
    {
        IDLE = 0,
        MOVE,
        ATTACK, // 1, 2, 3 추가 해야함 일단 테스트용
        SUMMON,
        GROGGY,
        DEAD,

        Phase2_END
    };
}
