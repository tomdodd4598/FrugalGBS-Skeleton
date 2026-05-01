cmake_minimum_required(VERSION 3.18)

project(Skeleton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SKELETON_N_VALUES 100 144)

find_package(FrugalGBS CONFIG REQUIRED)

option(SKELETON_ENABLE_GPU "Build Skeleton with the FrugalGBS GPU backend." ${FrugalGBS_ENABLE_CUDA})

if(SKELETON_ENABLE_GPU)
    if(NOT FrugalGBS_ENABLE_CUDA)
        message(FATAL_ERROR "SKELETON_ENABLE_GPU is ON, but the installed FrugalGBS package was built without CUDA support.")
    endif()

    include(CheckLanguage)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
        set(CMAKE_CUDA_STANDARD 17)
        set(CMAKE_CUDA_STANDARD_REQUIRED ON)
        if(NOT TARGET FrugalGBS::FrugalGBS_GPU)
            message(FATAL_ERROR "SKELETON_ENABLE_GPU is ON, but the FrugalGBS GPU target is not available from the installed package.")
        endif()
        set(SKELETON_BACKEND_TARGET FrugalGBS::FrugalGBS_GPU)
    else()
        message(FATAL_ERROR "SKELETON_ENABLE_GPU is ON, but no CUDA compiler was found for the Skeleton GPU bridge.")
    endif()
else()
    set(SKELETON_BACKEND_TARGET FrugalGBS::FrugalGBS_CPU)
endif()

add_executable(Skeleton
    src/helpers.cpp
    src/skeleton.cpp
)
target_include_directories(Skeleton PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Skeleton PRIVATE ${SKELETON_BACKEND_TARGET})

if(SKELETON_ENABLE_GPU)
    fgbs_add_gpu_instantiation(skeleton_gpu_bridge
        TARGETS
            Skeleton
        N_VALUES
            ${SKELETON_N_VALUES}
    )
endif()
