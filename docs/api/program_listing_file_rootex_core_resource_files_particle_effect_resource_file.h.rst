
.. _program_listing_file_rootex_core_resource_files_particle_effect_resource_file.h:

Program Listing for File particle_effect_resource_file.h
========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_particle_effect_resource_file.h>` (``rootex\core\resource_files\particle_effect_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   #include "resource_file.h"
   
   #include "Effekseer.h"
   
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
