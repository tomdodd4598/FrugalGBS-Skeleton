#include "helpers.hpp"

#if FGBS_GPU_ENABLED
#include "fgbs/gpu/gpu_utils.cuh"
#endif

namespace skeleton {

void prelude() {
    #if FGBS_GPU_ENABLED
    fgbs::gpu::set_default_device_ids();
    #endif
}
    
}
