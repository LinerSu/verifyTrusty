add_executable(user_iovec_to_membuf
  ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/iovec.c
  user_iovec_to_membuf_harness.c)
target_include_directories(user_iovec_to_membuf
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/include/uapi/uapi/
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/shared/lk
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/
)
sea_link_libraries(user_iovec_to_membuf sea_common_proofs.ir)
sea_overlink_libraries(user_iovec_to_membuf arch_copy_from_user_override.ir)
sea_attach_bc_link(user_iovec_to_membuf)
sea_add_unsat_test(user_iovec_to_membuf)