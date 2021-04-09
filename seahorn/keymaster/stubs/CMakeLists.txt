
add_library(keymaster_ipc_lib
  ${TRUSTY_ROOT}/trusty/user/app/keymaster/ipc/keymaster_ipc.cpp)
target_compile_definitions(keymaster_ipc_lib PRIVATE main=_main)
target_include_directories(keymaster_ipc_lib PRIVATE
  ${TRUSTY_ROOT}/trusty/user/app/keymaster
  ${TRUSTY_ROOT}/trusty/user/app/keymaster/ipc
  ${TRUSTY_ROOT}/system/keymaster/include)
sea_attach_bc_cc(keymaster_ipc_lib)

# Externalize static functions
# C++ static function transfer to name with _Z<name>v
set(LLVMIR_OPT ${SEA_PP})
llvmir_attach_opt_pass_target(TARGET keymaster_ipc_lib.opt.ir DEPENDS keymaster_ipc_lib.ir
  "--externalize-function=^_ZL*" "-externalize-functions-delete=false" "--externalize-fns")