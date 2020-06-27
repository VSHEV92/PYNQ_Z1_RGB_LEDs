# LED4
set_property -dict { PACKAGE_PIN N15  IOSTANDARD LVCMOS33 } [get_ports { GPIO_0_0_tri_io[0] }]; #led4_r
set_property -dict { PACKAGE_PIN G17  IOSTANDARD LVCMOS33 } [get_ports { GPIO_0_0_tri_io[1] }]; #led4_g
set_property -dict { PACKAGE_PIN L15  IOSTANDARD LVCMOS33 } [get_ports { GPIO_0_0_tri_io[2] }]; #led4_b

# LED5
set_property -dict { PACKAGE_PIN M15  IOSTANDARD LVCMOS33 } [get_ports { GPIO_0_0_tri_io[3] }]; #led5_r
set_property -dict { PACKAGE_PIN L14  IOSTANDARD LVCMOS33 } [get_ports { GPIO_0_0_tri_io[4] }]; #led5_g
set_property -dict { PACKAGE_PIN G14  IOSTANDARD LVCMOS33 } [get_ports { GPIO_0_0_tri_io[5] }]; #led5_b


