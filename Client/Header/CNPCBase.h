#pragma once

#include "CCharacterBase.h"

// 모든 NPC들의 기본 베이스 (NPC 춘식이, 춘봉이 등 노예 추가 예정)
class CNPCBase : public CCharacterBase
{
protected:
    explicit CNPCBase(DEVICE graphicDev);
    explicit CNPCBase(const CNPCBase& rhs);
    virtual ~CNPCBase();

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;

protected:
    void Free() override;

};
