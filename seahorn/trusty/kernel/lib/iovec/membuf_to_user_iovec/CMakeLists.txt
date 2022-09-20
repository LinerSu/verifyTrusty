add_executable(membuf_to_user_iovec
  ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/iovec.c
  membuf_to_user_iovec_harness.c)
target_include_directories(membuf_to_user_iovec
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/include/uapi/uapi/
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/shared/lk
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/
)
sea_link_libraries(membuf_to_user_iovec sea_common_proofs.ir)
sea_overlink_libraries(membuf_to_user_iovec arch_copy_from_user_override.ir)
sea_overlink_libraries(membuf_to_user_iovec arch_copy_to_user_override.ir)
sea_attach_bc_link(membuf_to_user_iovec)
sea_add_unsat_test(membuf_to_user_iovec)