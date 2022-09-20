add_executable(tipc_ns_read_ops
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  ${TRUSTY_ROOT}/trusty/user/app/storage/tipc_ns.c
  ${TRUSTY_ROOT}/external/arm-trusted-firmware/lib/libc/strlen.c # add strlen
  tipc_ns_read_ops_harness.c)
target_include_directories(tipc_ns_read_ops 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
)
sea_link_libraries(tipc_ns_read_ops sea_storage_ipc_proofs.ir)
sea_link_libraries(tipc_ns_read_ops sea_common_proofs.ir)
sea_overlink_libraries(tipc_ns_read_ops realloc_override.ir)
sea_overlink_libraries(tipc_ns_read_ops fprintf_override.ir)
sea_attach_bc_link(tipc_ns_read_ops)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(tipc_ns_read_ops)