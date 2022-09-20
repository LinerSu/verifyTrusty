add_library(arch_copy_from_user_override
  arch_copy_from_user_override.c
)
target_include_directories(arch_copy_from_user_override
  PRIVATE ${TRUSTY_ROOT}/external/lk/include/shared/lk/
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/
)
sea_attach_bc(arch_copy_from_user_override)


add_library(arch_copy_to_user_override
  arch_copy_to_user_override.c
)
target_include_directories(arch_copy_to_user_override
  PRIVATE ${TRUSTY_ROOT}/external/lk/include/shared/lk/
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/
)
sea_attach_bc(arch_copy_to_user_override)

add_library(mutex_override
  mutex_override.c
)
target_include_directories(mutex_override
  PRIVATE ${TRUSTY_ROOT}/external/lk/include/shared/lk/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/libc/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/io/include/
  PRIVATE ${TRUSTY_ROOT}/external/lk/lib/heap/include/
  PRIVATE ${PROJECT_SOURCE_DIR}/seahorn/trusty/kernel/lib/include/
)
target_compile_definitions(mutex_override PRIVATE -DSMP_MAX_CPUS=4) 
sea_attach_bc(mutex_override)