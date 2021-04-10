#include "obj3d.h"

ProcessMode Obj3d::getMode() const
{
    return mode;
}

void Obj3d::setMode(const ProcessMode &value)
{
    mode = value;
}
