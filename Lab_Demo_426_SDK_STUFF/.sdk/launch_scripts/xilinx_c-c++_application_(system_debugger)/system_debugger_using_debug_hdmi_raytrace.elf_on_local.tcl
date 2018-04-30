connect -url tcp:127.0.0.1:3121
source C:/Users/jerger/Desktop/Zybo-Z7-10-HDMI-master/Zybo-Z7-10-HDMI/sdk/system_wrapper_hw_platform_0/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351A6C7A0A"} -index 0
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351A6C7A0A" && level==0} -index 1
fpga -file C:/Users/jerger/Desktop/Zybo-Z7-10-HDMI-master/Zybo-Z7-10-HDMI/sdk/system_wrapper_hw_platform_0/system_wrapper.bit
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351A6C7A0A"} -index 0
loadhw -hw C:/Users/jerger/Desktop/Zybo-Z7-10-HDMI-master/Zybo-Z7-10-HDMI/sdk/system_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351A6C7A0A"} -index 0
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351A6C7A0A"} -index 0
dow C:/Users/jerger/Desktop/Zybo-Z7-10-HDMI-master/Zybo-Z7-10-HDMI/sdk/hdmi_raytrace/Debug/hdmi_raytrace.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351A6C7A0A"} -index 0
con
