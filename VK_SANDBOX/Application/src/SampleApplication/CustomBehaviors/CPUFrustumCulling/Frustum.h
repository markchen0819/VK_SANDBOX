#pragma once
#include "Plane.h"

struct Frustum
{
    Frustum() {};

    Plane topFace;
    Plane bottomFace;
    Plane rightFace;
    Plane leftFace;
    Plane farFace;
    Plane nearFace;
};
