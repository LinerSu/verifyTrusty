add_executable(rpmb_program_key
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  rpmb_program_key_harness.c)
target_include_directories(rpmb_program_key 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
target_compile_definitions(rpmb_program_key PRIVATE RPMB_PROTOCOL=RPMB_PROTOCOL_MMC)
sea_link_libraries(rpmb_program_key sea_storage_ipc_proofs.ir)
sea_link_libraries(rpmb_program_key sea_common_proofs.ir)
sea_link_libraries(rpmb_program_key rpmb.opt.ir)
sea_overlink_libraries(rpmb_program_key rpmb_mac_override.ir)
sea_overlink_libraries(rpmb_program_key realloc_override.ir)
sea_overlink_libraries(rpmb_program_key fprintf_override.ir)
sea_attach_bc_link(rpmb_program_key)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(rpmb_program_key)