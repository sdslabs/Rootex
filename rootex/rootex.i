%module Rootex
%{
#include "common/types.h"
#include "core/resource_loader.h"
#include "os/timer.h"
%}

%include "std_string.i"

namespace DirectX { namespace Colors {} }

%include "common/types.h"

%include "core/resource_loader.h"
%include "core/resource_file.h"
%include "os/timer.h"
