#pragma once
#include "CBase.h"
#include "Engine_Define.h"

struct ProtoKey
{
    COMPONENTTYPE type;
    wstring       tag;

    bool operator<(const ProtoKey& other) const
    {
        if (type != other.type)
            return type < other.type;
        return tag < other.tag;
    }
};

BEGIN(Engine)
class CComponent;

class ENGINE_DLL CProtoMgr : public CBase
{
    DECLARE_SINGLETON(CProtoMgr)

private:
    CProtoMgr();
    ~CProtoMgr() override;

public:
    HRESULT     Ready_Prototype(COMPONENTTYPE componentType, CComponent* component);
    HRESULT     Ready_Prototype(COMPONENTTYPE componentType, const wstring& tag, CComponent* component);
    CComponent* Clone_Prototype(COMPONENTTYPE componentType, const wstring& tag);
    CComponent* Clone_Prototype(COMPONENTTYPE componentType);

private:
    CComponent* Find_Prototype(COMPONENTTYPE componentType, const wstring& tag);
    CComponent* Find_Prototype(COMPONENTTYPE componentType);

private:
    map<ProtoKey, CComponent*> m_Prototypes;

private:
    void Free() override;
};

END
