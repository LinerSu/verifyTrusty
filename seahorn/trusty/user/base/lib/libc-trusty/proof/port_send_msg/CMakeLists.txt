add_executable(port_send_msg_pf main.c)

sea_attach_bc_link(port_send_msg_pf)
sea_add_unsat_test(port_send_msg_pf)
