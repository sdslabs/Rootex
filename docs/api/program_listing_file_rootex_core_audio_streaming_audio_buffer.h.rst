
.. _program_listing_file_rootex_core_audio_streaming_audio_buffer.h:

Program Listing for File streaming_audio_buffer.h
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_audio_streaming_audio_buffer.h>` (``rootex\core\audio\streaming_audio_buffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #define BUFFER_COUNT 50
   #define MAX_BUFFER_QUEUE_LENGTH BUFFER_COUNT / 25
   
   #include "audio_buffer.h"
   #include "framework/systems/audio_system.h"
   
   class StreamingAudioBuffer : public AudioBuffer
   {
       ALuint m_Buffers[BUFFER_COUNT];
       ALsizei m_BufferSize;
   
       const char* m_BufferCursor;
       int m_BufferQueueLength;
       const char* m_BufferEnd;
   
       void initializeBuffers() override;
       void destroyBuffers() override;
   
   public:
       StreamingAudioBuffer(Ref<AudioResourceFile> audioFile);
       ~StreamingAudioBuffer();
   
       void loadNewBuffers(int count, bool isLooping);
   
       ALuint* getBuffers();
       int getBufferQueueLength();
   };
