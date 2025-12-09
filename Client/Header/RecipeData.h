#pragma once
#include "pch.h"
enum class RECIPETYPE
{
    COOK,
    BUILD
};
struct Ingredient
{
    int id;
    int count;
};

struct Recipe
{
    int         id;
    RECIPETYPE type;
    string name;
    string desc;
    string UIFileName;
    string UIPath;
    int  quality;
    int  price;
    int  hunger;

   vector<Ingredient> ingredients;
};
