#pragma once

#include "CMapObjectBase.h"
#include "CMapObject_Grass.h"

static CMapObjectBase* CreateMapObjectType(DEVICE graphicDev, MAPOBJTYPE type)
{
    switch (type)
    {
    case MAPOBJTYPE::GRASS:
        return CMapObject_Grass::Create(graphicDev);
    case MAPOBJTYPE::RUBBLE:
        break;
     //return CMapObject_   
    case MAPOBJTYPE::DECO:
        break;
    }
}
