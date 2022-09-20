add_executable(port_connect_pf main.c)

sea_attach_bc_link(port_connect_pf)
sea_add_unsat_test(port_connect_pf)
