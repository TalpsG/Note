module bcd7reg (
    input [3:0] num,
    output reg [7:0] seg
);
    always@(*) begin
        case(num)
            4'd0: seg=8'hc0;
            4'd1: seg=8'hf9;
            4'd2: seg=8'ha4;
            4'd3: seg=8'hb0;
            4'd4: seg=8'h99;
            4'd5: seg=8'h92;
            4'd6: seg=8'h82;
            4'd7: seg=8'hf8;
            4'd8: seg=8'h80;
            4'd9: seg=8'h90;
            4'd10: seg=8'h88;
            4'd11: seg=8'h83;
            4'd12: seg=8'hc6;
            4'd13: seg=8'ha1;
            4'd14: seg=8'h86;
            4'd15: seg=8'h8e;
            default: seg='1;
        endcase
    end
endmodule
