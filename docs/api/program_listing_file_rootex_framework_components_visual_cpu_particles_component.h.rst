
.. _program_listing_file_rootex_framework_components_visual_cpu_particles_component.h:

Program Listing for File cpu_particles_component.h
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_cpu_particles_component.h>` (``rootex\framework\components\visual\cpu_particles_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "core/renderer/materials/particles_material.h"
   #include "model_component.h"
   
   #define MAX_PARTICLES 50000
   
   struct ParticleTemplate
   {
       Vector3 velocity = { 1.0f, 0.0f, 0.0f };
       Color colorBegin = ColorPresets::Red;
       Color colorEnd = ColorPresets::Blue;
       float velocityVariation = 10.0f;
       float angularVelocityVariation = 0.5f;
       float sizeBegin = 0.1f;
       float sizeEnd = 0.0f;
       float sizeVariation = 0.1f;
       float lifeTime = 1.0f;
   };
   
   class CPUParticlesComponent : public ModelComponent
   {
       static Component* Create(const JSON::json& componentData);
   
       Vector<InstanceData> m_InstanceBufferData;
       Vector<InstanceData> m_InstanceBufferLiveData;
       int m_LiveParticlesCount;
       Ptr<VertexBuffer> m_InstanceBuffer;
   
       struct Particle
       {
           bool isActive = false;
           float sizeBegin;
           float sizeEnd;
           float lifeTime;
           float lifeRemaining;
           Color colorBegin;
           Color colorEnd;
           Vector3 velocity;
           Vector3 angularVelocity;
       };
   
       ParticleTemplate m_ParticleTemplate;
       Vector<Particle> m_ParticlePool;
       Ref<ParticlesMaterial> m_ParticlesMaterial;
       size_t m_PoolIndex;
       int m_EmitRate;
   
       enum class EmitMode : int
       {
           Point = 0,
           Square = 1,
           Cube = 2
       };
   
       EmitMode m_CurrentEmitMode;
       Vector3 m_EmitterDimensions;
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::CPUParticlesComponent;
   
       CPUParticlesComponent(size_t poolSize, const String& particleModelPath, const String& materialPath, const ParticleTemplate& particleTemplate, bool visibility, unsigned int renderPass, EmitMode emitMode, int emitRate, const Vector3& emitterDimensions);
       CPUParticlesComponent(CPUParticlesComponent&) = delete;
       virtual ~CPUParticlesComponent() = default;
   
       virtual bool setupData() override;
       virtual bool preRender(float deltaMilliseconds) override;
       virtual void render() override;
   
       void setMaterial(Ref<ParticlesMaterial> particlesMaterial);
       void emit(const ParticleTemplate& particleTemplate);
       void expandPool(const size_t& poolSize);
   
       virtual const char* getName() const override { return "CPUParticlesComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
