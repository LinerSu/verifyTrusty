add_executable(rpmb_read
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  rpmb_read_harness.c)
target_include_directories(rpmb_read 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
target_compile_definitions(rpmb_read PRIVATE RPMB_PROTOCOL=RPMB_PROTOCOL_MMC)
sea_link_libraries(rpmb_read sea_storage_ipc_proofs.ir)
sea_link_libraries(rpmb_read sea_common_proofs.ir)
sea_link_libraries(rpmb_read rpmb.opt.ir)
sea_overlink_libraries(rpmb_read rpmb_mac_override.ir)
sea_overlink_libraries(rpmb_read realloc_override.ir)
sea_overlink_libraries(rpmb_read fprintf_override.ir)
sea_attach_bc_link(rpmb_read)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(rpmb_read)