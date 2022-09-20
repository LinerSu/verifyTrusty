add_executable(storage_event_loops
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  event_loops_harness.c)
target_include_directories(storage_event_loops 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
  )
sea_link_libraries(storage_event_loops sea_storage_ipc_proofs.ir)
# sea_overlink_libraries(storage_event_loops trusty_api_override.ir)
sea_overlink_libraries(storage_event_loops realloc_override.ir)
sea_overlink_libraries(storage_event_loops fprintf_override.ir)
sea_overlink_libraries(storage_event_loops ipc_loop_override.ir)
sea_attach_bc_link(storage_event_loops)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(storage_event_loops)