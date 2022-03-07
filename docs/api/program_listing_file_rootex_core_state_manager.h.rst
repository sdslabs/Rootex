
.. _program_listing_file_rootex_core_state_manager.h:

Program Listing for File state_manager.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_state_manager.h>` (``rootex\core\state_manager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   class StateManager;
   
   class State
   {
   public:
       virtual ~State() = default;
   
       virtual void update(StateManager& manager, float deltaMilliseconds) {};
   
       virtual void enter(StateManager& manager) { }
       virtual Ptr<State> exit(StateManager& manager, float deltaMilliseconds) = 0;
   };
   
   class StateManager
   {
   protected:
       Ptr<State> m_CurrentState;
   
       StateManager(Ptr<State>&& currentState);
       virtual ~StateManager() = default;
   
   public:
       virtual void update(float deltaMilliseconds);
   
       void transition(Ptr<State> newState);
   };
