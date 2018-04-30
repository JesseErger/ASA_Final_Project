`timescale 1ns / 1ps

module Discriminant_test();    
    parameter HalfClk = 10;
    
    // test inputs
    logic signed [31:0] sphereX, sphereY, sphereZ;
    logic [31:0] sphereRadius;
    logic signed [31:0] rayX, rayY, rayZ;
    logic signed [31:0] rayDX, rayDY, rayDZ;
    logic CLK, areset, inputValid;
    
    // test outputs
    wire intersects;
    wire [31:0] discriminant, b_out;
    wire inputReady, dataOutWrite;
    
    DiscriminantCalculator calc(
        .SphereX(sphereX),
        .SphereY(sphereY),
        .SphereZ(sphereZ),
        .SphereRadius(sphereRadius),
        .RayStartX(rayX),
        .RayStartY(rayY),
        .RayStartZ(rayZ),
        .RayDirX(rayDX),
        .RayDirY(rayDY),
        .RayDirZ(rayDZ),
        .CLK(CLK),
        .aresetn(areset),
        .QuickIntersects(intersects),
        .Discriminant(discriminant),
        .B_out(b_out),
        .InputValid(inputValid),
        .InputReady(inputReady),
        .OutputReady(dataOutWrite)
    );
        
    // generate a clock signal
    initial begin
        forever #(HalfClk) CLK = ~CLK; 
    end
         
    // useful macros
    `define waitForReady \
        $display("Waiting for InputReady"); \
        while(inputReady != 1'b1) begin \
            #(HalfClk * 2); \
        end
    
    `define waitForOutValid \
        $display("Waiting for OutputReady"); \
        while ( dataOutWrite == 1'b0 ) begin \
            #(HalfClk * 2); \
        end
    
    `define displayOut \
        while ( dataOutWrite == 1'b1 ) begin \
            $display("intersects? %d, discriminant=%d, b=%d", intersects, discriminant, b_out); \
            #(HalfClk * 2); \
        end
        
    initial begin        
        CLK = 1'b0;
        inputValid = 1'b0;
        // reset
        areset = 1'b0;
        #(HalfClk * 5);
        // reset finished
        areset = 1'b1;
        #(HalfClk *2);
        
        // engine should start in ready state
        if (inputReady != 1'b1) $fatal("not ready");
       
        // first test
        $display("sphere radius 2 at (0, 0, 0) and a ray from (0, 0, -10) with direction (0, 0, 1)");
        sphereX = 0;
        sphereY = 0;
        sphereZ = 0;
        sphereRadius = 2;
        rayX = 0;
        rayY = 0;
        rayZ = -10;
        rayDX = 0;
        rayDY = 0;
        rayDZ = 1;
        
        // wait until InputReady
        `waitForReady
        inputValid = 1'b1;       
        // wait 1 cycle to make sure input is captured
        #(HalfClk * 2);
        inputValid = 1'b0;
        
        // wait until we see DataOutWrite go to 1
        `waitForOutValid
        // display the result
        $display("intersects? %d, discriminant=%d, b=%d", intersects, discriminant, b_out);
        assert(intersects == 1);
        assert(discriminant == 16);
        assert(b_out == 32'hffffffec); // -20 with 32 bits signed
        #(HalfClk * 2);
        
        // second test
        $display("sphere radius 2 at (10, -10, 10) and a ray from (0, 10, 0) with direction (0, -1, 0)");
        sphereX = 10;
        sphereY = -10;
        sphereZ = 10;
        sphereRadius = 2;
        rayX = 0;
        rayY = 10;
        rayZ = 0;
        rayDX = 0;
        rayDY = -1;
        rayDZ = 0;
                       
        // wait until InputReady
        `waitForReady
        inputValid = 1'b1;       
        // wait 1 cycle to make sure input is captured
        #(HalfClk * 2);
        inputValid = 1'b0;
        
        // wait until we see DataOutWrite go to 1
        `waitForOutValid
        // display the result
        $display("intersects? %d, discriminant=%d, b=%d", intersects, discriminant, b_out);
        assert(intersects == 0);
        assert(discriminant == 32'hfffffcf0); // -784 with 32 bits signed
        assert(b_out == 32'hffffffd8); // -40 with 32 bits signed
        #(HalfClk * 2);
          
        
        // third test
        $display("sphere radius 3 at (10, 10, 0) and a ray from (0, 0, 0) with direction (1, 1, 0)");
        sphereX = 10;
        sphereY = 10;
        sphereZ = 0;
        sphereRadius = 3;
        rayX = 0;
        rayY = 0;
        rayZ = 0;
        rayDX = 1;
        rayDY = 1;
        rayDZ = 0;
                               
        // wait until InputReady
        `waitForReady
        inputValid = 1'b1;       
        // wait 1 cycle to make sure input is captured
        #(HalfClk * 2);
        inputValid = 1'b0;
        
        // wait until we see DataOutWrite go to 1
        `waitForOutValid
        // display the result
        $display("intersects? %d, discriminant=%d, b=%d", intersects, discriminant, b_out);
        assert(intersects == 1);
        assert(discriminant == 32'h00000048);
        assert(b_out == 32'hffffffd8); // -40 with 32 bits signed
        #(HalfClk * 2);
          
        $finish;
    end
    
endmodule