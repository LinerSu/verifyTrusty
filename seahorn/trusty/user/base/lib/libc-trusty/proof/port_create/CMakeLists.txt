add_executable(port_create_pf main.c)

sea_attach_bc_link(port_create_pf)
sea_add_unsat_test(port_create_pf)
