
.. _program_listing_file_rootex_core_resource_files_audio_resource_file.h:

Program Listing for File audio_resource_file.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_audio_resource_file.h>` (``rootex\core\resource_files\audio_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   
   #include "al.h"
   #include "AL/alut.h"
   
   class AudioResourceFile : public ResourceFile
   {
       ALenum m_Format;
       float m_Frequency;
       int m_BitDepth;
       int m_Channels;
       float m_Duration;
   
       const char* m_DecompressedAudioBuffer;
       ALsizei m_AudioDataSize;
   
       explicit AudioResourceFile(const FilePath& path);
   
       friend class ResourceLoader;
   
       void resetBuffer();
   
   public:
       explicit AudioResourceFile(AudioResourceFile&) = delete;
       explicit AudioResourceFile(AudioResourceFile&&) = delete;
       ~AudioResourceFile();
   
       void reimport() override;
   
       const char* getAudioData() const { return m_DecompressedAudioBuffer; }
       ALsizei getAudioDataSize() const { return m_AudioDataSize; }
       ALenum getFormat() const { return m_Format; }
       float getFrequency() const { return m_Frequency; }
       int getBitDepth() const { return m_BitDepth; }
       int getChannels() const { return m_Channels; }
       float getDuration() const { return m_Duration; }
   };
