#ifndef PARTICLES_MATERIAL_HLSLI
#define PARTICLES_MATERIAL_HLSLI

#include "register_locations_pixel_shader.h"

struct ParticlesMaterial
{
    int isLit;
    float specularIntensity;
    float specPow;
    float reflectivity;
    float refractionConstant;
    float refractivity;
    int affectedBySky;
    int hasNormalMap;
};

#endif
