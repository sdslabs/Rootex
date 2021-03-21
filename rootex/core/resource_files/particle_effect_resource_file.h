#pragma once

#include "common/types.h"
#include "resource_file.h"

#include "Effekseer.h"

/// An Effekseer effect file used in particle effects.
class ParticleEffectResourceFile : public ResourceFile
{
	explicit ParticleEffectResourceFile(const FilePath& path);

	Effekseer::Effect* m_Effect;

	friend class ResourceLoader;

public:
	explicit ParticleEffectResourceFile(const ParticleEffectResourceFile&) = delete;
	explicit ParticleEffectResourceFile(const ParticleEffectResourceFile&&) = delete;
	~ParticleEffectResourceFile() = default;

	Effekseer::Effect* getEffect() const { return m_Effect; };

	void reimport() override;
};
