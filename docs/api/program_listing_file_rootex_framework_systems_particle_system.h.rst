
.. _program_listing_file_rootex_framework_systems_particle_system.h:

Program Listing for File particle_system.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_particle_system.h>` (``rootex\framework\systems\particle_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <codecvt>
   
   #include "system.h"
   
   #include "Effekseer.h"
   #include "EffekseerRendererDX11.h"
   #include "EffekseerSoundAL.h"
   
   class ParticleSystem : public System
   {
       static inline std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> s_Convert;
   
       EffekseerRendererDX11::Renderer* m_Renderer = nullptr;
       Effekseer::Manager* m_Manager = nullptr;
       EffekseerSound::Sound* m_Sound = nullptr;
   
       float m_TargetUPS;
   
       ParticleSystem();
       ParticleSystem(const ParticleSystem&) = delete;
       ~ParticleSystem();
   
   public:
       static ParticleSystem* GetSingleton();
   
       Effekseer::Handle play(Effekseer::Effect* effect, const Vector3& position, int startFrame);
       void stop(Effekseer::Handle handle);
   
       void setMatrix(Effekseer::Handle handle, const Matrix& mat);
       void setSpeed(Effekseer::Handle handle, const float& speed);
       void setTargetLocation(Effekseer::Handle handle, const Vector3& target);
   
       bool getPaused(Effekseer::Handle handle);
   
       Effekseer::Effect* loadEffect(const String& path);
       void release(Effekseer::Effect* effect);
   
       bool initialize(const JSON::json& systemData) override;
       void begin() override;
       void update(float deltaMilliseconds) override;
   };
