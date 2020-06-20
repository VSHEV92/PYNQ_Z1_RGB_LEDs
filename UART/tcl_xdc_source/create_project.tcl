# если проект уже существует, удаляем его
if {[file exist ../PYNQ_Z1_RGB_LEDs_UART]} {
    file delete -force ../PYNQ_Z1_RGB_LEDs_UART
}

# создаем проект и указываем board file
create_project PYNQ_Z1_RGB_LEDs_UART ../PYNQ_Z1_RGB_LEDs_UART -part xc7z020clg400-1
set_property board_part www.digilentinc.com:pynq-z1:part0:1.0 [current_project]

# настраиваем ip cache каталого
config_ip_cache -import_from_project -use_cache_location ../ip_cache
update_ip_catalog

# добавляем xdc file с LOC constraints
add_files -fileset constrs_1 -norecurse LOC.xdc

# создаем block design, добавляем zynq и делаем block automation
create_bd_design "zynq_bd"
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]

# удаляем всю переферию, которая добавлена по умолчанию
set_property -dict [list CONFIG.PCW_USE_M_AXI_GP0 {0} CONFIG.PCW_EN_CLK0_PORT {0} CONFIG.PCW_EN_RST0_PORT {0} CONFIG.PCW_QSPI_PERIPHERAL_ENABLE {0} CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} CONFIG.PCW_ENET0_PERIPHERAL_ENABLE {0} CONFIG.PCW_SD0_PERIPHERAL_ENABLE {0} CONFIG.PCW_UART0_PERIPHERAL_ENABLE {0} CONFIG.PCW_USB0_PERIPHERAL_ENABLE {0} CONFIG.PCW_GPIO_MIO_GPIO_ENABLE {0}] [get_bd_cells processing_system7_0]

# добавляем UART_0 и устанавливаем baud rate в 9600
set_property -dict [list CONFIG.PCW_UART0_BAUD_RATE {9600} CONFIG.PCW_UART0_PERIPHERAL_ENABLE {1} CONFIG.PCW_UART0_UART0_IO {MIO 14 .. 15}] [get_bd_cells processing_system7_0]

# добаляем GPIO EMIO для управления светодиодами и делаем его внешним портом
set_property -dict [list CONFIG.PCW_GPIO_EMIO_GPIO_ENABLE {1} CONFIG.PCW_GPIO_EMIO_GPIO_IO {6}] [get_bd_cells processing_system7_0]
make_bd_intf_pins_external  [get_bd_intf_pins processing_system7_0/GPIO_0]

# проверяем, сохраняем и закрываем block design
validate_bd_design
regenerate_bd_layout
save_bd_design
close_bd_design [get_bd_designs zynq_bd]

# создаем hdl_wrapper
make_wrapper -files [get_files ../PYNQ_Z1_RGB_LEDs_UART/PYNQ_Z1_RGB_LEDs_UART.srcs/sources_1/bd/zynq_bd/zynq_bd.bd] -top
add_files -norecurse ../PYNQ_Z1_RGB_LEDs_UART/PYNQ_Z1_RGB_LEDs_UART.srcs/sources_1/bd/zynq_bd/hdl/zynq_bd_wrapper.v
