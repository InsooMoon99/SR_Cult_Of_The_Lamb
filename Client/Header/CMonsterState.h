#pragma once
#include "CBase.h"
#include "Engine_Define.h"


namespace Engine
{
    class CTransform;
    class CTexture;
}

class CPlayer;

class CMonsterState :
    public CBase
{
protected:
    explicit CMonsterState();
    virtual ~CMonsterState();

public:
    virtual void Enter() ;
    virtual void Exit() ;


public:
     virtual HRESULT Ready_State() { return S_OK; }
     virtual _int    Update_State(const _float& timeDelta);
     virtual void    LateUpdate_State(const _float& timeDelta) {}
     virtual void    Render_State() {}

public:
    virtual _uint Get_StateKey() const { return m_dwCurStateKey; }

protected:
    _uint m_dwCurStateKey;

    _bool m_bStarted;
    _bool m_bExited;
    _bool m_bCanTransit;


// 여기까지 스테이트 필수 //

protected:
    virtual void    Find_Player();
protected:
    CPlayer* m_pPlayer;
    CTransform* m_pPlayerTransformCom;
    CTransform* m_pOwnerTransformCom;
    //CTexture* m_pOwnerTextureCom

    _float m_fCurStateElapsedTime; // 누적시간
    _float      m_fMinStateTime;
    _bool  m_bMinStateTimePassed;

private:
    // CBase을(를) 통해 상속됨
    //void Free() override;
};

