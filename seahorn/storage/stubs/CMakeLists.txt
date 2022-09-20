add_library(ipc_loop_override ipc_loop_override.c)
target_include_directories(ipc_loop_override 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_attach_bc(ipc_loop_override)

add_library(storage_ipc
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c)
# target_compile_options(storage_ipc PRIVATE "-include${PROJECT_BINARY_DIR}/include/config.h")
sea_attach_bc(storage_ipc)

set(LLVMIR_OPT ${SEA_PP})
# Change linkage to all functions that start with 'handle_' to external
# This is necessary so that they can be called directly from verification harness
llvmir_attach_opt_pass_target(TARGET storage_ipc.opt.ir DEPENDS storage_ipc.ir
  "--externalize-function=^handle_*" "-externalize-functions-delete=false" "--externalize-fns")

add_library(rpmb
  ${TRUSTY_ROOT}/trusty/user/app/storage/rpmb.c)
target_compile_definitions(rpmb PRIVATE RPMB_PROTOCOL=RPMB_PROTOCOL_MMC)
sea_attach_bc(rpmb)

# Change linkage to all functions that start with 'rpmb_mac' to external
# This is necessary so that they can be called directly from verification harness
set(LLVMIR_OPT ${SEA_PP})
llvmir_attach_opt_pass_target(TARGET rpmb.opt.ir DEPENDS rpmb.ir
  "--externalize-function=^rpmb_mac" "-externalize-functions-delete=false" "--externalize-fns")

add_library(rpmb_mac_override rpmb_mac_override.c)
target_include_directories(rpmb_mac_override 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_attach_bc(rpmb_mac_override)