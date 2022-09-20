add_executable(chan_msg_sync main.c)

sea_attach_bc_link(chan_msg_sync)
sea_add_unsat_test(chan_msg_sync)
