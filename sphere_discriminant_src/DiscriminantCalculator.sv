`timescale 1ns / 1ps

module DiscriminantCalculator(
    input signed [15:0] SphereX,
    input signed [15:0] SphereY,
    input signed [15:0] SphereZ,
    input [15:0] SphereRadius,
    input signed [15:0] RayStartX,
    input signed [15:0] RayStartY,
    input signed [15:0] RayStartZ,
    input signed [15:0] RayDirX,
    input signed [15:0] RayDirY,
    input signed [15:0] RayDirZ,
    output signed [15:0] Discriminant,
    output signed [15:0] B_out,
    output QuickIntersects,
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
    parameter process2 = 2'b11;
        
    // state control
    logic [1:0] nextState;
    logic [1:0] state;
        
    // input variables
    logic signed [15:0] dx;
    logic signed [15:0] dy;
    logic signed [15:0] dz;
    
    // output variables
    logic signed [15:0] discriminant;
                  
    // internal logic variables
    logic signed [15:0] A;
    logic signed [15:0] B;
    logic signed [15:0] C;
    logic signed [15:0] distX;
    logic signed [15:0] distY;
    logic signed [15:0] distZ;
    
    // temp variables
    logic [15:0] sphereRadiusSquared;
    
    // output logic
    assign InputReady = (state == waiting);
    assign OutputReady = (state == process2);
    assign Discriminant = discriminant;
    assign B_out = B;
    assign QuickIntersects = (discriminant > 0);
        
    always_ff@(posedge CLK) begin
        if (aresetn == 1'b0) begin
            state <= waiting;
        end else begin
            state <= nextState;
        end
            
        // handle state specific logic        
        if (nextState == reading) begin
            dx <= RayDirX;
            dy <= RayDirY;
            dz <= RayDirZ;
            
            // some simple calcuations
            distX <= RayStartX - SphereX;
            distY <= RayStartY - SphereY;
            distZ <= RayStartZ - SphereZ;
            
            sphereRadiusSquared <= SphereRadius * SphereRadius;
            A <= RayDirX * RayDirX + RayDirY * RayDirY + RayDirZ * RayDirZ;
        end else if (nextState == process) begin
            B <= (dx * distX + dy * distY + dz * distZ) << 1;
            C <= (distX * distX + distY * distY + distZ * distZ) - sphereRadiusSquared;
        end else if (nextState == process2) begin
            discriminant <= B * B - 4 * A * C;
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
            nextState = process2;
        process2:
            nextState = waiting;
        endcase
    end
endmodule