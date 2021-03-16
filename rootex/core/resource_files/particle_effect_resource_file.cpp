#include "particle_effect_resource_file.h"

#include "systems/particle_system.h"

ParticleEffectResourceFile::ParticleEffectResourceFile(const FilePath& path)
    : ResourceFile(Type::ParticleEffect, path)
{
	reimport();
}

void ParticleEffectResourceFile::reimport()
{
	m_Effect = ParticleSystem::GetSingleton()->loadEffect(m_Path.generic_string());
}
