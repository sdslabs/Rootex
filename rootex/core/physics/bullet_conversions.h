#pragma once

#include "common/types.h"

#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"

/// Helpers for conversion to and from Bullet's data types.
btTransform MatTobtTransform(Matrix const& mat);
Matrix BtTransformToMat(btTransform const& trans);
btVector3 VecTobtVector3(Vector3 const& vec3);
Vector3 BtVector3ToVec(btVector3 const& btvec);
