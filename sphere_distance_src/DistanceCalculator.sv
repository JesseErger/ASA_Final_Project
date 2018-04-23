`timescale 1ns / 1ps

module DistanceCalculator(
    input [23:0] RootDiscriminant,
    input signed [31:0] B,   
    input QuickIntersects,
    input [31:0] OldDistance,
    output Intersects,
    output [31:0] Distance,
    input CLK,
    input aresetn,
    input InputValid,
    output InputReady,
    output OutputReady
    );
    
    // state encodings
    parameter waiting = 2'b00;
    parameter reading = 2'b01;
    parameter process = 2'b10;
        
    // state control
    logic [1:0] nextState;
    logic [1:0] state;
        
    // input variables
    logic [31:0] oldDistance;
    
    // output variables
    logic isects;
    logic [31:0] distance;
                  
    // internal logic variables
    logic signed [31:0] t0;
    logic signed [31:0] t1;
    
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
                    // implement with floating points as x < epsilon
                    if (t1 <= 0 || t1 >= oldDistance) begin
                        isects <= 1'b0;
                        distance <= oldDistance;
                    end else begin
                        distance <= t1;
                    end
                end else begin
                    if (t0 <= 0 || t0 >= oldDistance) begin
                        isects <= 1'b0;
                        distance <= oldDistance;
                    end else begin
                        distance <= t0;
                    end
                end
            end else begin
                distance <= oldDistance;
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
