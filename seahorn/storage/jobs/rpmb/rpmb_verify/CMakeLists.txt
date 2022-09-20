add_executable(rpmb_verify
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  rpmb_verify_harness.c)
target_include_directories(rpmb_verify 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
target_compile_definitions(rpmb_verify PRIVATE RPMB_PROTOCOL=RPMB_PROTOCOL_MMC)
sea_link_libraries(rpmb_verify sea_storage_ipc_proofs.ir)
sea_link_libraries(rpmb_verify sea_common_proofs.ir)
sea_link_libraries(rpmb_verify rpmb.opt.ir)
sea_overlink_libraries(rpmb_verify rpmb_mac_override.ir)
sea_overlink_libraries(rpmb_verify realloc_override.ir)
sea_overlink_libraries(rpmb_verify fprintf_override.ir)
sea_attach_bc_link(rpmb_verify)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(rpmb_verify)