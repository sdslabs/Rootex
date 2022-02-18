
.. _program_listing_file_rootex_framework_components_audio_short_music_component.h:

Program Listing for File short_music_component.h
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_audio_short_music_component.h>` (``rootex\framework\components\audio\short_music_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "audio_component.h"
   
   #include "core/audio/audio_source.h"
   #include "core/audio/static_audio_buffer.h"
   #include "core/event_manager.h"
   #include "core/resource_loader.h"
   
   class ShortMusicComponent : public AudioComponent
   {
       COMPONENT(ShortMusicComponent, Category::Audio);
   
       Ref<StaticAudioSource> m_StaticAudioSource;
       Ref<StaticAudioBuffer> m_StaticAudioBuffer;
       Ref<AudioResourceFile> m_AudioFile;
   
   public:
       ShortMusicComponent(Entity& owner, const JSON::json& data);
       ~ShortMusicComponent();
   
       AudioResourceFile* getAudioFile() const { return m_AudioFile.get(); }
       void setAudioFile(Ref<AudioResourceFile> audioFile);
   
       bool setupData() override;
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(ShortMusicComponent);
