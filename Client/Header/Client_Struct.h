#pragma once


typedef struct tagBossData
{
    _vec3 startpos;


} BossConfig;


struct SpriteBounceDesc
{
    // 일단 바운스 기준으로 필요 한거
    _vec3 pos;// 위치
    _vec3 velocity;// 초기 힘
    float groundY = 0.f;// 땅 위치
    float gravity = -980.f;
    float restitution = 0.5f; // 튕김 계수
    float damping = 0.9f; // 수평 속도 감쇠
    float m_DampingPerSecond = 0.98f; // 공기저항

};
