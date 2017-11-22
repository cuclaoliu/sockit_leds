# sockit_leds

Light up Arrow SoCKit board user fpga leds.

Based on https://github.com/arrow-socfpga/arrow-sockit-ghrd/tree/sockit-ghrd-16.1.

Just keep quartus project files, e.g. sockit_ghrd.qpf, sockit_ghrd.qsf, sockit_ghrd.sdc, and design files, e.g. sockit_ghrd.qsys, verilog files, and IPs.

In qsys, connect button_pio, dispsw_pio, and led_pio etc. directly to h2f_lw_axi_master, and reassign base address.
