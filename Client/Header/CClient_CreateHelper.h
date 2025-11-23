#pragma once

#include "CMapObjectBase.h"
#include "CMapObject_Grass.h"

static CMapObjectBase* CreateMapObjectType(DEVICE graphicDev, MAPOBJTYPE type)
{
    switch (type)
    {
    case MAPOBJTYPE::GRASS:
        return CMapObject_Grass::Create(graphicDev);
    case MAPOBJTYPE::ROCK:
        // 일단은 Grass로 테스트하기
        break;
    }
}
