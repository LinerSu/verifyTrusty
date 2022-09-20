add_executable(storage_multiple_ports
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  multiple_ports_harness.c)
target_include_directories(storage_multiple_ports 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
  )
sea_link_libraries(storage_multiple_ports sea_storage_ipc_proofs.ir)
sea_overlink_libraries(storage_multiple_ports realloc_override.ir)
sea_overlink_libraries(storage_multiple_ports fprintf_override.ir)
sea_overlink_libraries(storage_multiple_ports ipc_loop_override.ir)
sea_attach_bc_link(storage_multiple_ports)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(storage_multiple_ports)