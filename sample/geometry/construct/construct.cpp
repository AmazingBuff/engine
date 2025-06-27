//
// Created by AmazingBuff on 2025/6/23.
//
#include <common.h>

int main()
{
    BVHTree tree;
    Primitive* primitive = new Sphere({0, 0, 0},  4);

    tree.insert(std::move(primitive));
}