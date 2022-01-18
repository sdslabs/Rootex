#include "particle_system.h"

#include "components/visual/effect/particle_effect_component.h"
#include "renderer/rendering_device.h"
#include "systems/render_system.h"

Effekseer::Matrix44 MatrixToEffekseer44(const Matrix& m)
{
	Effekseer::Matrix44 em;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			em.Values[i][j] = m.m[i][j];
		}
	}
	return em;
}

Effekseer::Matrix43 MatrixToEffekseer43(const Matrix& m)
{
	Effekseer::Matrix43 em;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			em.Value[i][j] = m.m[i][j];
		}
	}
	return em;
}

Effekseer::Vector3D Vector3ToEffekseer(const Vector3& v)
{
	return { v.x, v.y, v.z };
}

ParticleSystem::ParticleSystem()
    : System("ParticleSystem", UpdateOrder::Render, true)
    , m_TargetUPS(0.0f)
{
}

ParticleSystem::~ParticleSystem()
{
	m_Manager->Destroy();
	m_Renderer->Destroy();
	m_Sound->Destroy();
}

ParticleSystem* ParticleSystem::GetSingleton()
{
	static ParticleSystem singleton;
	return &singleton;
}

bool ParticleSystem::initialize(const JSON::json& systemData)
{
	m_Renderer = EffekseerRendererDX11::Renderer::Create(
	    RenderingDevice::GetSingleton()->getDevice(),
	    RenderingDevice::GetSingleton()->getContext(),
	    systemData["maxSquares"]);
	if (!m_Renderer)
	{
		ERR("Could not initialize Effekseer renderer");
		return false;
	}

	m_Manager = Effekseer::Manager::Create(systemData["maxSquares"]);

	m_Manager->SetSpriteRenderer(m_Renderer->CreateSpriteRenderer());
	m_Manager->SetRibbonRenderer(m_Renderer->CreateRibbonRenderer());
	m_Manager->SetRingRenderer(m_Renderer->CreateRingRenderer());
	m_Manager->SetTrackRenderer(m_Renderer->CreateTrackRenderer());
	m_Manager->SetModelRenderer(m_Renderer->CreateModelRenderer());

	m_Manager->SetTextureLoader(m_Renderer->CreateTextureLoader());
	m_Manager->SetModelLoader(m_Renderer->CreateModelLoader());
	m_Manager->SetMaterialLoader(m_Renderer->CreateMaterialLoader());

	m_Manager->CreateCullingWorld(
	    systemData["culling"]["size"]["x"],
	    systemData["culling"]["size"]["y"],
	    systemData["culling"]["size"]["z"],
	    systemData["culling"]["layerCount"]);

	m_Sound = EffekseerSound::Sound::Create(systemData["maxVoices"]);
	if (!m_Sound)
	{
		ERR("Could not initialize Effekseer sound");
		return false;
	}
	m_Manager->SetSoundPlayer(m_Sound->CreateSoundPlayer());
	m_Manager->SetSoundLoader(m_Sound->CreateSoundLoader());

	m_TargetUPS = systemData["targetUPS"];

	return true;
}

void ParticleSystem::begin()
{
	for (auto& pec : ECSFactory::GetAllParticleEffectComponent())
	{
		if (pec.isPlayOnStart())
		{
			pec.play();
		}
	}
}

void ParticleSystem::update(float deltaMilliseconds)
{
	CameraComponent* camera = RenderSystem::GetSingleton()->getCamera();
	const Effekseer::Matrix44& cameraProj = MatrixToEffekseer44(camera->getProjectionMatrix());

	m_Renderer->SetProjectionMatrix(cameraProj);
	m_Renderer->SetCameraMatrix(MatrixToEffekseer44(camera->getViewMatrix()));
	if (!(m_IsSystemPaused))
	{
		for (auto& pec : ECSFactory::GetAllParticleEffectComponent())
		{
			if (pec.isMoving())
			{
				setMatrix(pec.getHandle(), pec.getTransformComponent()->getAbsoluteTransform());
			}
		}
	}
	else
	{
		for (auto& pec : ECSFactory::GetAllParticleEffectComponent())
		{
			if (!(pec.getOwner().getScene()->getIsScenePaused()))
			{
				if (pec.isMoving())
				{
					setMatrix(pec.getHandle(), pec.getTransformComponent()->getAbsoluteTransform());
				}
			}
		}
	}

	m_Manager->Update(m_TargetUPS * (deltaMilliseconds * MS_TO_S));

	RenderingDevice::GetSingleton()->setAlphaBS();
	RenderingDevice::GetSingleton()->setTemporaryUIRS();
	m_Renderer->BeginRendering();
	m_Manager->CalcCulling(cameraProj, false);
	m_Manager->Draw();
	m_Renderer->EndRendering();
}

Effekseer::Handle ParticleSystem::play(Effekseer::Effect* effect, const Vector3& position, int startFrame)
{
	return m_Manager->Play(effect, { position.x, position.y, position.z }, startFrame);
}

void ParticleSystem::stop(Effekseer::Handle handle)
{
	m_Manager->StopEffect(handle);
}

void ParticleSystem::setMatrix(Effekseer::Handle handle, const Matrix& mat)
{
	m_Manager->SetMatrix(handle, MatrixToEffekseer43(mat));
}

void ParticleSystem::setSpeed(Effekseer::Handle handle, const float& speed)
{
	m_Manager->SetSpeed(handle, speed);
}

void ParticleSystem::setTargetLocation(Effekseer::Handle handle, const Vector3& target)
{
	m_Manager->SetTargetLocation(handle, { target.x, target.y, target.z });
}

bool ParticleSystem::getPaused(Effekseer::Handle handle)
{
	return m_Manager->GetPaused(handle);
}

Effekseer::Effect* ParticleSystem::loadEffect(const String& path)
{
	std::wstring uPath = s_Convert.from_bytes(path);
	Effekseer::Effect* effect = Effekseer::Effect::Create(m_Manager, (const char16_t*)uPath.c_str(), 1.0f);
	if (!effect)
	{
		WARN("Could not load effect: " + path);
	}
	return effect;
}

void ParticleSystem::release(Effekseer::Effect* effect)
{
	effect->Release();
}
