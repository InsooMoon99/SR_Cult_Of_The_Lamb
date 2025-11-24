#pragma once

#include "CProtoMgr.h"
#include "CEnumHelper.h"
#include "CManagement.h"
#include "CScene.h"

BEGIN(Engine)
class CGameObject;

template <typename T>
static T* CreateProtoComponent(CGameObject* Owner, COMPONENTTYPE eComponentType, const wstring& tag = L"")
{
    if (Owner == nullptr)
        return nullptr;

    auto proto = Engine::CProtoMgr::GetInstance()->Clone_Prototype(eComponentType, tag);
    if (proto == nullptr)
        return nullptr;

    T* component = dynamic_cast<T*>(proto);

    if (component == nullptr)
    {
        Safe_Release(proto);
        return nullptr;
    }

    component->Set_Owner(Owner);
    component->Set_Type(eComponentType);

    const char* tempChar = ToString(eComponentType);
    wstring componentName = CharToWString(tempChar);

    component->Set_Name(componentName);

    return component;
}

static void SpawnAttackObject(CGameObject* obj)
{
    auto mgr = CManagement::GetInstance();
    auto scene = mgr ? mgr->Get_Scene() : nullptr;

    if (scene == nullptr)
        return;

    scene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, obj, OBJTYPE::ATTACK_COL);
}


END
