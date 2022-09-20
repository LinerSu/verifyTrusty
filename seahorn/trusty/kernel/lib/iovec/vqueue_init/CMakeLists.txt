add_executable(vqueue_init
  ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/vqueue.c
  ${TRUSTY_ROOT}/trusty/kernel/lib/extmem/external_memory.c
  ${TRUSTY_ROOT}/trusty/kernel/lib/sm/shared_mem.c
  ${TRUSTY_ROOT}/trusty/kernel/lib/sm/ns_mem.c
  ${TRUSTY_ROOT}/trusty/kernel/lib/sm/sm.c
  ${TRUSTY_ROOT}/external/lk/kernel/vm/vmm.c
  ${TRUSTY_ROOT}/external/lk/kernel/mutex.c
  ${TRUSTY_ROOT}/external/lk/kernel/vm/pmm.c
  ${TRUSTY_ROOT}/external/lk/kernel/vm/vm.c
  ${TRUSTY_ROOT}/external/lk/lib/binary_search_tree/binary_search_tree.c
  vqueue_init_harness.c)

if (${TRUSTY_TARGET} STREQUAL "x86_64")
  target_sources(vqueue_init PRIVATE ${TRUSTY_ROOT}/external/lk/arch/x86/64/mmu.c)
endif()

target_include_directories(vqueue_init
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/shared/lk
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/trusty/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/include/uapi/uapi/
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/include/shared/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/binary_search_tree/include/
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/rand/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/libc/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/heap/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/io/include/
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/extmem/include/
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/sm/include/
  PRIVATE ${TRUSTY_ROOT}/external/headers/include/
  PRIVATE ${TRUSTY_ROOT}/trusty/kernel/lib/version/include/
)
target_compile_definitions(vqueue_init PRIVATE -DSMP_MAX_CPUS=4 -DWITH_KERNEL_VM=1 -DLK_LOGLEVEL=2 -DKERNEL_BASE=0x80000000 -DMEMBASE=0x00000000) 
sea_link_libraries(vqueue_init sea_common_proofs.ir)
sea_overlink_libraries(vqueue_init mutex_override.ir)
sea_attach_bc_link(vqueue_init)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(vqueue_init)