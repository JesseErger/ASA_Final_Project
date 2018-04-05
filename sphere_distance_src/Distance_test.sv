`timescale 1ns / 1ps

module Distance_test();    
    parameter HalfClk = 10;
    
    // test inputs
    logic [15:0] rootDiscriminant;
    logic signed [15:0] B;
    logic quickIntersects;
    logic [15:0] oldDistance;
    logic CLK, areset, inputValid;
    
    // test outputs
    wire intersects;
    wire [15:0] distance;
    wire inputReady, dataOutWrite;
    
    DistanceCalculator calc(
       .RootDiscriminant(rootDiscriminant),
       .B(B),   
       .QuickIntersects(quickIntersects),
       .OldDistance(oldDistance),
       .CLK(CLK),
       .aresetn(areset),
       .Intersects(intersects),
       .Distance(distance),
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
        $display("from the first step, discriminant is 16, B is -20, and intersects is true");
        rootDiscriminant = 4;
        B = 16'hffec; // -20 with 16 bits signed
        quickIntersects = 1'b1;
        oldDistance = 1000; // set the initial distance to some arbitrary high value
        
        // wait until InputReady
        `waitForReady
        inputValid = 1'b1;       
        // wait 1 cycle to make sure input is captured
        #(HalfClk * 2);
        inputValid = 1'b0;
        
        // wait until we see DataOutWrite go to 1
        `waitForOutValid
        // display the result
        $display("intersects? %d, distance=%d", intersects, distance);
        assert(intersects == 1);
        assert(distance == 8);
        #(HalfClk * 2);
        
        // second test
        $display("sphere radius 2 at (10, -10, 10) and a ray from (0, 10, 0) with direction (0, -1, 0)");
        $display("from the first step, discriminant is -784, B is -40, and intersects is false");
        rootDiscriminant = 0; // some error value
        B = 16'hffd8; // -40 with 16 bits signed
        quickIntersects = 1'b0;
        oldDistance = 1000;
 
        // wait until InputReady
        `waitForReady
        inputValid = 1'b1;       
        // wait 1 cycle to make sure input is captured
        #(HalfClk * 2);
        inputValid = 1'b0;
        
        // wait until we see DataOutWrite go to 1
        `waitForOutValid
        // display the result
        $display("intersects? %d, distance=%d", intersects, distance);
        assert(intersects == 0);
        // don't care about distance since there is no intersection
        #(HalfClk * 2);
                
        // third test - currently incorrect
        $display("sphere radius 3 at (10, 10, 0) and a ray from (0, 0, 0) with direction (1, 1, 0)");
        $display("from the first step, discriminant is 72, B is -40, and intersects is true");
        rootDiscriminant = 72;
        B = 16'hffd8; // -40 with 16 bits signed
        quickIntersects = 1'b1;
        oldDistance = 1000;
 
        // wait until InputReady
        `waitForReady
        inputValid = 1'b1;       
        // wait 1 cycle to make sure input is captured
        #(HalfClk * 2);
        inputValid = 1'b0;
        
        // wait until we see DataOutWrite go to 1
        `waitForOutValid
        // display the result
        $display("intersects? %d, distance=%d", intersects, distance);
        assert(intersects == 1);
        // don't care about distance since there is no intersection
        #(HalfClk * 2);
                                      
        $finish;
    end
    
endmodule
