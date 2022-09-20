add_executable(rpmb_send
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  rpmb_send_harness.c)
target_include_directories(rpmb_send 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
sea_link_libraries(rpmb_send sea_storage_ipc_proofs.ir)
sea_link_libraries(rpmb_send sea_common_proofs.ir)
sea_overlink_libraries(rpmb_send realloc_override.ir)
sea_overlink_libraries(rpmb_send fprintf_override.ir)
sea_attach_bc_link(rpmb_send)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(rpmb_send)