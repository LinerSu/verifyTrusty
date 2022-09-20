add_executable(port_read_msg_pf main.c)

sea_attach_bc_link(port_read_msg_pf)
sea_add_unsat_test(port_read_msg_pf)
