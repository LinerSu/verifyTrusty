add_executable(rpmb_write
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  rpmb_write_harness.c)
target_include_directories(rpmb_write 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
target_compile_definitions(rpmb_write PRIVATE RPMB_PROTOCOL=RPMB_PROTOCOL_MMC)
sea_link_libraries(rpmb_write sea_storage_ipc_proofs.ir)
sea_link_libraries(rpmb_write sea_common_proofs.ir)
sea_link_libraries(rpmb_write rpmb.opt.ir)
sea_overlink_libraries(rpmb_write rpmb_mac_override.ir)
sea_overlink_libraries(rpmb_write realloc_override.ir)
sea_overlink_libraries(rpmb_write fprintf_override.ir)
sea_attach_bc_link(rpmb_write)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(rpmb_write)