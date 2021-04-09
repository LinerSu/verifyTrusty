include_directories(${TRUSTY_ROOT}/system/iot/attestation/atap/)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/system_state/include/)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/system_state/include/)
include_directories(${TRUSTY_ROOT}/external/nanopb-c/)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/keybox/client/include/)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/keybox/include/)
include_directories(${TRUSTY_ROOT}/external/boringssl/src/include/)
include_directories(${TRUSTY_ROOT}/external/libcppbor/include/cppbor/)

string(APPEND CMAKE_CXX_FLAGS " -std=c++17")

add_subdirectory(stubs)
add_subdirectory(jobs/ipc)