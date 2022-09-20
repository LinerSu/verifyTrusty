add_executable(port_wait_pf main.c)

sea_attach_bc_link(port_wait_pf)
sea_add_unsat_test(port_wait_pf)
