add_executable(rpmb_read_no_mac
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  rpmb_read_no_mac_harness.c)
target_include_directories(rpmb_read_no_mac 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
target_compile_definitions(rpmb_read_no_mac PRIVATE RPMB_PROTOCOL=RPMB_PROTOCOL_MMC)
sea_link_libraries(rpmb_read_no_mac sea_storage_ipc_proofs.ir)
sea_link_libraries(rpmb_read_no_mac sea_common_proofs.ir)
sea_link_libraries(rpmb_read_no_mac rpmb.opt.ir)
sea_overlink_libraries(rpmb_read_no_mac rpmb_mac_override.ir)
sea_overlink_libraries(rpmb_read_no_mac realloc_override.ir)
sea_overlink_libraries(rpmb_read_no_mac fprintf_override.ir)
sea_attach_bc_link(rpmb_read_no_mac)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(rpmb_read_no_mac)