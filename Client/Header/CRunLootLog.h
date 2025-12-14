#pragma once
using namespace std;
#include "ItemInstance.h"
#include <vector>

class CRunLootLog
{
    struct Slot
    {
        ItemInstance itemInst;
        int          count;
    };
public:
    CRunLootLog();
    ~CRunLootLog();

public:
    void Clear();
    void Add(const ItemInstance& itemInst,int count =1 );

private:
    vector<Slot> m_ItemSlot;

};

