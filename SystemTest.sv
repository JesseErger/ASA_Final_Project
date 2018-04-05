`timescale 1ns / 1ps

module SystemTest();
    logic tb_ACLK;
    logic tb_ARESETn;
    
    wire temp_clk;
    wire temp_rstn; 
    
    logic [31:0] read_data;
    logic resp;
    //------------------------------------------------------------------------
    // Simple Clock Generator
    //------------------------------------------------------------------------
    
    always begin 
        tb_ACLK= 1'b0; #10; tb_ACLK = 1'b1; #10;
    end
    
    initial 
    begin
    `ifndef XILINX_SIMULATOR
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.M_AXI_GP0.master.IF.PC.fatal_to_warnings=1;
        #40;
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.M_AXI_GP0.master.IF.PC.fatal_to_warnings=0;
     `endif         
    end
       
    initial
    begin    
        $display ("running the tb");
        
        tb_ARESETn = 1'b0;
        repeat(2) @(posedge tb_ACLK);        
        tb_ARESETn = 1'b1;
        @(posedge tb_ACLK);
        
        repeat(5) @(posedge tb_ACLK);
          
        //Reset the PL - Counter gets reset as well
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.fpga_soft_reset(32'h1);
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.fpga_soft_reset(32'h0);
    
        repeat(5) @(posedge tb_ACLK);        
        $display("sphere radius 2 at (0, 0, 0) and a ray from (0, 10, 0) with direction (0, -1, 0)");
        
        // wait for InputReady
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00018,4,read_data,resp);
        @(posedge tb_ACLK);   
        while (read_data != 1) begin
            SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00018,4,read_data,resp);
            @(posedge tb_ACLK);
        end

        // write sphere x and y
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00000,4,32'h00000000,resp);
        // write sphere z and r
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00004,4,32'h00000002,resp);
        // write ray start x and y
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00008,4,32'h0000000a,resp);
        // write ray start z and direction x
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C0000C,4,32'h00000000,resp);
        // write ray direction y and z
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00010,4,32'hFFFF0000,resp);
        // change the InputValid signal
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00018,1,4'h1,resp);
        
        // wait for ready
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00018,4,read_data,resp);
        while (read_data != 1) begin
            @(posedge tb_ACLK);
            SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00018,4,read_data,resp);
        end
             
        // check if intersects
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00014,4,read_data,resp);
        $display("intersects? %d", read_data);
        
        // prepare to send the second half        
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C1000C,4,read_data,resp);
        while (read_data != 1) begin
            @(posedge tb_ACLK);
            SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C1000C,4,read_data,resp);
        end
        
        // write the old distance
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C10000,4,32'h0000ffff,resp);        
        // change the InputValid signal
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C1000C,1,4'h1,resp);
        
        // wait for ready
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C1000C,4,read_data,resp);
        while (read_data != 1) begin
            @(posedge tb_ACLK);
            SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C1000C,4,read_data,resp);
        end
        
        // read Intersects and Distance        
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C10004,4,read_data,resp);
        $display("intersects? %d", read_data);
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C10008,4,read_data,resp);
        $display("distance=%d", read_data);
        
        // second test        
        @(posedge tb_ACLK);        
        $display("sphere radius 2 at (10, -10, 10) and a ray from (0, 10, 0) with direction (0, -1, 0)");
        // write sphere x and y
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00000,4,32'h00A0FFF6,resp);
        // write sphere z and r
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00004,4,32'h00A00002,resp);
        // write ray start x and y
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00008,4,32'h0000000A,resp);
        // write ray start z and direction x
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C0000C,4,32'h00000000,resp);
        // write ray direction y and z
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00010,4,32'hFFFF0000,resp);
        // change the InputValid signal
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C00018,1,4'h2,resp);
        
        // wait for ready
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00018,4,read_data,resp);
        while (read_data != 1) begin
            @(posedge tb_ACLK);
            SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00018,4,read_data,resp);
        end
             
        // check if intersects
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C00014,4,read_data,resp);
        $display("intersects? %d", read_data);
        
        // write the old distance
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C10000,4,32'h0000ffff,resp);        
        // change the InputValid signal
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.write_data(32'h43C1000C,1,4'h2,resp);
        
        // wait for ready
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C1000C,4,read_data,resp);
        while (read_data != 1) begin
            @(posedge tb_ACLK);
            SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C1000C,4,read_data,resp);
        end
        
        // read Intersects and Distance        
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C10004,4,read_data,resp);
        $display("intersects? %d", read_data);
        SystemTest.zynq_sys.System_i.processing_system7_0.inst.read_data(32'h43C10008,4,read_data,resp);
        $display("distance=%d", read_data);
                
        $finish;
    end
    
    assign temp_clk = tb_ACLK;
    assign temp_rstn = tb_ARESETn;
    
    System_wrapper zynq_sys(
        .DDR_addr(),
        .DDR_ba(),
        .DDR_cas_n(),
        .DDR_ck_n(),
        .DDR_ck_p(),
        .DDR_cke(),
        .DDR_cs_n(),
        .DDR_dm(),
        .DDR_dq(),
        .DDR_dqs_n(),
        .DDR_dqs_p(),
        .DDR_odt(),
        .DDR_ras_n(),
        .DDR_reset_n(),
        .DDR_we_n(),
        .FIXED_IO_ddr_vrn(),
        .FIXED_IO_ddr_vrp(),
        .FIXED_IO_mio(),
        .FIXED_IO_ps_clk(temp_clk),
        .FIXED_IO_ps_porb(temp_rstn),
        .FIXED_IO_ps_srstb(temp_rstn)
    );

endmodule
