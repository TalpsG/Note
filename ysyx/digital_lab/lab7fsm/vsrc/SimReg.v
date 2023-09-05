module SimReg #(
    DATA_WIDTH=4
) (
    input clk,reset,
    input [DATA_WIDTH-1:0] in,
    output reg [3:0] out
);
    always@(posedge clk) begin
        if(reset==1) out=4'b0;
        else out = in;
    end
    
endmodule