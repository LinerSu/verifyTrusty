add_executable(keymaster_ipc
${TRUSTY_ROOT}/trusty/user/app/keymaster/trusty_keymaster.cpp
${TRUSTY_ROOT}/trusty/user/app/keymaster/ipc/keymaster_generic_access_policy.cpp
${TRUSTY_ROOT}/trusty/user/app/keymaster/trusty_keymaster_context.cpp
harness.cpp)
# target_compile_definitions(keymaster_ipc PRIVATE 
#   GATEKEEPER_STORAGE_PORT=STORAGE_CLIENT_TP_PORT)
target_include_directories(keymaster_ipc PRIVATE 
  ${TRUSTY_ROOT}/trusty/user/app/keymaster
  ${TRUSTY_ROOT}/trusty/user/app/keymaster/ipc
  ${TRUSTY_ROOT}/system/keymaster/include)

sea_link_libraries(keymaster_ipc keymaster_ipc_lib.opt.ir)
sea_overlink_libraries(keymaster_ipc trusty_api_override.ir)
sea_overlink_libraries(keymaster_ipc realloc_override.ir)
sea_overlink_libraries(keymaster_ipc fprintf_override.ir)
sea_overlink_libraries(keymaster_ipc time_override.ir)
sea_overlink_libraries(keymaster_ipc new_delete_override.ir)
sea_attach_bc_cc_link(keymaster_ipc)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(keymaster_ipc)