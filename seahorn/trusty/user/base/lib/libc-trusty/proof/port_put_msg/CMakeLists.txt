add_executable(port_put_msg_pf main.c)

sea_attach_bc_link(port_put_msg_pf)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(port_put_msg_pf)
