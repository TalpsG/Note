module shift(
    input clk,in,
    input [2:0]mod,
    output reg [7:0] data
);
    always@(posedge clk) begin
        case(mod)
            3'd0:data <= 0;
            3'd1:data <= '1;
            3'd2:data <= {1'b0,data[7:1]};
            3'd3:data <= {data[6:0],1'b0};
            3'd4:data <= {data[7],data[7:1]};
            3'd5:data <= {in,data[7:1]};
            3'd6:data <= {data[0],data[7:1]};
            3'd7:data <= {data[6:0],data[7]};
            default: data<=0;
        endcase
    end
endmodule