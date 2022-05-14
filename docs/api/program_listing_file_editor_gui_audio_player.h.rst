
.. _program_listing_file_editor_gui_audio_player.h:

Program Listing for File audio_player.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_audio_player.h>` (``editor\gui\audio_player.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "resource_loader.h"
   #include "audio/static_audio_buffer.h"
   #include "audio/audio_source.h"
   #include "os/timer.h"
   
   class AudioPlayer
   {
       float m_FractionProgress = 0.0f;
       Ref<AudioResourceFile> m_OpenFile = nullptr;
       Ref<StaticAudioBuffer> m_Buffer;
       Ref<StaticAudioSource> m_Source;
       bool m_Looping = false;
   
       StopTimer m_Timer;
   
       void drawFileInfo();
   
   public:
       AudioPlayer() = default;
       AudioPlayer(AudioPlayer&) = delete;
       ~AudioPlayer() = default;
   
       Ref<ResourceFile> load(const FilePath& filePath);
       void unload();
       void draw(float deltaMilliseconds);
   };
