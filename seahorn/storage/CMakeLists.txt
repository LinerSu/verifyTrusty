include_directories(include)
add_subdirectory(lib)
add_subdirectory(stubs)

# IPC
add_subdirectory(jobs/ipc/indirect_handlers)
add_subdirectory(jobs/ipc/msg_buffer)
add_subdirectory(jobs/ipc/port_create_destroy)
add_subdirectory(jobs/ipc/multiple_ports)

# rpmb
add_subdirectory(jobs/rpmb/rpmb_send)
add_subdirectory(jobs/rpmb/rpmb_program_key)
add_subdirectory(jobs/rpmb/rpmb_read)
add_subdirectory(jobs/rpmb/rpmb_read_no_mac)
add_subdirectory(jobs/rpmb/rpmb_write)
add_subdirectory(jobs/rpmb/rpmb_verify)

# tipc_ns
add_subdirectory(jobs/tipc_ns/open_file)
add_subdirectory(jobs/tipc_ns/open_close_file)
add_subdirectory(jobs/tipc_ns/read_ops)
add_subdirectory(jobs/tipc_ns/write_ops)