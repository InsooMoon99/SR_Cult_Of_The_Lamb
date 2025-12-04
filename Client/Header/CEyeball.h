#pragma once

#include "CMonsterBase.h"

class CMonsterController;

BEGIN(Engine)
class CTexture;
END

class CEyeball : public CMonsterBase
{
public:
    enum PATTERN
    {
        EYEBALL_START,
        EYEBALL_START_EXIT,
        EYEBALL_IDLE,
        EYEBALL_EXIT,
        EYEBALL_ENTER,
        EYEBALL_GROUND,
        EYEBALL_MOVE,
        EYEBALL_ATTACK,
        EYEBALL_DEAD,

        EYEBALL_END
    };

private:
    explicit CEyeball(DEVICE graphicDev);
    explicit CEyeball(const CEyeball& rhs);
    ~CEyeball() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    using OnEyeDeadCallback = function<void()>;

    OnEyeDeadCallback FEyeDeadCallback = nullptr;

    void Set_OnDeathCallback(OnEyeDeadCallback callback)
    {
        FEyeDeadCallback = callback;
    }

private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

public:
    CMonsterController* Get_Controller() { return m_EyeballController; }
    const char* Get_StateName(_int stateNum);

    static CEyeball* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CMonsterController* m_EyeballController;

    
};

