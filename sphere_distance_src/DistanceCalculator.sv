`timescale 1ns / 1ps

module DistanceCalculator(
    input [15:0] RootDiscriminant,
    input signed [15:0] B,   
    input QuickIntersects,
    input [15:0] OldDistance,
    output Intersects,
    output [15:0] Distance,
    input CLK,
    input aresetn,
    input InputValid,
    output InputReady,
    output OutputReady
    );
    
    // state encodings
    parameter waiting = 2'b000;
    parameter reading = 2'b001;
    parameter process = 2'b010;
        
    // state control
    logic [1:0] nextState;
    logic [1:0] state;
        
    // input variables
    logic [15:0] oldDistance;
    
    // output variables
    logic isects;
    logic [15:0] distance;
                  
    // internal logic variables
    logic signed [15:0] t0;
    logic signed [15:0] t1;
    
    // temp variables
    
    // output logic
    assign InputReady = (state == waiting);
    assign OutputReady = (state == process);
    assign Intersects = isects;
    assign Distance = distance;
        
    always_ff@(posedge CLK) begin
        if (aresetn == 1'b0) begin
            state <= waiting;
        end else begin
            state <= nextState;
        end
            
        // handle state specific logic        
        if (nextState == reading) begin
            isects <= QuickIntersects;
            oldDistance <= OldDistance;
            
            t0 <= (-B - RootDiscriminant) >> 1;
            t1 <= (-B + RootDiscriminant) >> 1;
        end else if (nextState == process) begin
            if (isects == 1'b1) begin
                if (t0 > t1) begin
                    // can we do the floating point check - and do we need to?
                    if (t1 < 0.001 || t1 >= oldDistance) begin
                        isects <= 1'b0;
                    end else begin
                        isects <= 1'b1;
                        distance <= t1;
                    end
                end else begin
                    if (t0 < 0.001 || t0 >= oldDistance) begin
                        isects <= 1'b0;
                    end else begin
                        isects <= 1'b1;
                        distance <= t0;
                    end
                end
            end
        end
    end
    
    // next state logic
    always_comb begin
        case (state) 
        waiting:
            if (InputValid) begin
                nextState = reading;
            end else begin
                nextState = waiting;
            end            
        reading:
            nextState = process;
        process:
            nextState = waiting;
        endcase
    end
endmodule
