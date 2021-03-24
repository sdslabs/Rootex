#include "particle_effect_resource_file.h"

#include "systems/particle_system.h"

ParticleEffectResourceFile::ParticleEffectResourceFile(const FilePath& path)
    : ResourceFile(Type::ParticleEffect, path)
    , m_Effect(nullptr)
{
	reimport();
}

void ParticleEffectResourceFile::reimport()
{
	if (m_Effect)
	{
		ParticleSystem::GetSingleton()->release(m_Effect);
	}
	m_Effect = ParticleSystem::GetSingleton()->loadEffect(m_Path.generic_string());
}
