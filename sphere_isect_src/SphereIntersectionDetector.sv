`timescale 1ns / 1ps

module SphereIntersectionDetector(
    input [31:0] SphereX,
    input [31:0] SphereY,
    input [31:0] SphereZ,
    input [31:0] SphereRadius,
    input [31:0] RayStartX,
    input [31:0] RayStartY,
    input [31:0] RayStartZ,
    input [31:0] RayDirX,
    input [31:0] RayDirY,
    input [31:0] RayDirZ,
    output Intersects,
    output [31:0] Distance,
    input CLK,
    input areset,
    input InputValid,
    output InputReady,
    output OutputReady
    );
    
    // state encodings
    parameter waiting = 3'b000;
    parameter reading = 3'b001;
    parameter process = 3'b010;
    parameter process2 = 3'b011;
    parameter process3 = 3'b100;
    parameter process4 = 3'b101;
    parameter writing = 3'b111;
        
    // state control
    logic [2:0] nextState;
    logic [2:0] state;
        
    // input variables
    logic [31:0] sx;
    logic [31:0] sy;
    logic [31:0] sz;
    logic [31:0] sr;
    logic [31:0] rx;
    logic [31:0] ry;
    logic [31:0] rz;
    logic [31:0] dx;
    logic [31:0] dy;
    logic [31:0] dz;
    
    // output variables
    logic isect;
    logic [31:0] distance;
                  
    // internal logic variables
    logic [31:0] A;
    logic [31:0] B;
    logic [31:0] C;
    logic [31:0] distX;
    logic [31:0] distY;
    logic [31:0] distZ;
    logic [31:0] discriminant;
    logic [31:0] rootDiscriminant;
    logic [31:0] t0;
    logic [31:0] t1;
    
    // temp variables
    logic [31:0] spherePosSquared;
    
    // output logic
    assign InputReady = (state == waiting);
    assign OutputReady = (state == writing);
    assign Intersects = isect;
    assign Distance = distance;
        
    always_ff@(posedge CLK) begin
        if (areset == 1'b0) begin
            state <= waiting;
        end else begin
            state <= nextState;
        end
            
        // handle state specific logic        
        if (nextState == reading) begin
            sx <= SphereX;
            sy <= SphereY;
            sz <= SphereX;
            rx <= RayStartX;
            ry <= RayStartY;
            rz <= RayStartZ;
            dx <= RayDirX;
            dy <= RayDirY;
            dz <= RayDirZ;
            isect <= 1'b0;
            
            // some simple calcuations
            distX <= RayStartX - SphereX;
            distY <= RayStartY - SphereY;
            distZ <= RayStartZ - SphereZ;
            
            spherePosSquared <= sx * sx + sy * sy + sz * sz;
            A <= rx * rx + ry * ry + rz * rz;
        end else if (nextState == process) begin
            B <= (rx * distX + ry * distY + rz * distZ) << 1;
            C <= (distX * distX + distY * distY + distZ * distZ) - spherePosSquared;
        end else if (nextState == process2) begin
            discriminant <= B * B - 4 * A * C;
             // Don't know if Sqrt is defined - may need to implement
            rootDiscriminant <= Sqrt(B * B - 4 * A * C);
        end else if (nextState == process3) begin
            isect <= 1'b1;
            t0 <= (-B + rootDiscriminant) >> 1;
            t1 <= (-B - rootDiscriminant) >> 1;
        end else if (nextState == process4) begin
            // gonna assume that intersection is larger than 0 and less than the original distance
            if (t0 > t1) begin
                t0 <= t1;
                distance <= t1;
            end else begin
                distance <= t0;
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
            nextState = process2;
        process2:
            if (discriminant > 0) begin
                nextState = process3;
            end else begin
                nextState = writing;
            end
        process3:
            nextState = process4;
        process4:
            nextState = writing;
        writing:
            nextState = waiting;
        endcase
    end
endmodule
