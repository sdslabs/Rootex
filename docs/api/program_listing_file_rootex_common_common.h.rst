
.. _program_listing_file_rootex_common_common.h:

Program Listing for File common.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_common_common.h>` (``rootex\common\common.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   #include "os/os.h"
   #include "script/interpreter.h"
   
   #define PRINT(m_Msg)                                      \
       {                                                     \
           String msg = String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::Print(msg);                                 \
       }
   #define WARN(m_Msg)                                                                                                 \
       {                                                                                                               \
           String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::PrintWarning(msg);                                                                                    \
       }
   #define ERR(m_Msg)                                                                                                  \
       {                                                                                                               \
           String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::PrintError(msg);                                                                                      \
       }
   #define PANIC(m_IfTtrue, m_Msg) \
       if (m_IfTtrue)              \
       {                           \
           WARN(m_Msg);            \
       }
   #if defined(DEBUG) || defined(_DEBUG)
   #define DEBUG_PANIC(m_IfTtrue, m_Msg) \
       if (m_IfTtrue)                    \
       {                                 \
           WARN(m_Msg);                  \
       }
   #else
   #define DEBUG_PANIC(m_IfTtrue, m_Msg)
   #endif
   #define ERR_CUSTOM(m_file, m_func, m_Msg)                                          \
       {                                                                              \
           String msg = String(__FILE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::PrintError(String(m_file) + ":" + String(m_func) + ": " + m_Msg);    \
       }
   
   #define PRINT_SILENT(m_Msg)                               \
       {                                                     \
           String msg = String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::PrintSilent(msg);                           \
       }
   #define WARN_SILENT(m_Msg)                                                                                          \
       {                                                                                                               \
           String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::PrintWarningSilent(msg);                                                                              \
       }
   #define ERR_SILENT(m_Msg)                                                                                           \
       {                                                                                                               \
           String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
           ::OS::PrintErrorSilent(msg);                                                                                \
       }
   #define PANIC_SILENT(m_IfTtrue, m_Msg) \
       if (m_IfTtrue)                     \
       {                                  \
           WARN_SILENT(m_Msg);            \
       }
   #define ERR_CUSTOM_SILENT(m_file, m_func, m_Msg)                                      \
       {                                                                                 \
           String msg = String(__FILE__) + ":" + String(__FUNCTION__) + ": " + m_Msg;    \
           ::OS::PrintErrorSilent(String(m_file) + ":" + String(m_func) + ": " + m_Msg); \
       }
