#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CTimer;

class ENGINE_DLL CTimerMgr : public CBase
{
    DECLARE_SINGLETON(CTimerMgr)

private:
    explicit CTimerMgr();
    ~CTimerMgr() override;

public:
    _float Get_TimeDelta(const wstring& timerTag);
    void   Set_TimeDelta(const wstring& timerTag);

    _float Get_TimeDelta();
    void   Set_TimeDelta();

public:
    HRESULT Ready_Timer(const wstring& timerTag);

private:
    CTimer* Find_Timer(const wstring& timerTag);

private:
    map<wstring, CTimer*> m_Timers;

private:
    void Free() override;
};

END
