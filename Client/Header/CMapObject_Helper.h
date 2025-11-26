#pragma once

#include "CMapObjectBase.h"
#include "CMapObject_Grass.h"
#include "CMapObject_Rubble.h"
#include "CMapObject_Deco.h"

static CMapObjectBase* CreateMapObjectType(DEVICE graphicDev, MAPOBJTYPE type)
{
    switch (type)
    {
    case MAPOBJTYPE::GRASS:
        return CMapObject_Grass::Create(graphicDev);
    case MAPOBJTYPE::RUBBLE:
        return CMapObject_Rubble::Create(graphicDev);
    case MAPOBJTYPE::DECO:
        return CMapObject_Deco::Create(graphicDev);
    }
}
