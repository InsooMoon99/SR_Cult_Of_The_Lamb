#pragma once
#include "CComponent.h"

/// @brief [NPC용] 상호작용 오브젝트 감지기 용도의 스태틱 컴포넌트
class CWorldInteractive;
class CConstructCircle;

class CInteractionChecker : public CComponent
{
private:
    explicit CInteractionChecker(DEVICE graphicDev);
    explicit CInteractionChecker(const CInteractionChecker& rhs);
    ~CInteractionChecker() override;

public:
    HRESULT Ready_InteractionChecker();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    CWorldInteractive* Get_NearestRock(const _vec3& srcPos);
    CWorldInteractive* Get_NearestTree(const _vec3& srcPos);
    CConstructCircle*  Get_NearestConstruction(const _vec3& srcPos);

public:
    static CInteractionChecker* Create(DEVICE graphicDev);
    CComponent*                 Clone() override;

protected:
    void Free() override;
};
