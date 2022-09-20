add_executable(port_wait_any_pf main.c)

sea_attach_bc_link(port_wait_any_pf)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(port_wait_any_pf)
