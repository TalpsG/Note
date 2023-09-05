module ALU(
    input [3:0]a,b,
    input [2:0] mod,
    output reg [3:0] res,
    output reg overflow,zero,carry
);
    reg [3:0]b_minus;
    always@(*)begin
        if(mod==1) b_minus = 4'b1111^b;
        else b_minus = b;
        case(mod)
            3'd0:begin
                // +
                {carry,res} =a+b_minus;
            end
            3'd1:begin
                //-
                {carry,res} = a+b_minus+1;
                carry = ~carry;
            end
            3'd2:begin
                // ~
                res = {4{1'b1}}^a;
                carry = 0;
            end
            3'd3:begin
                // and 
                res = a&b;
                carry = 0;
            end
            3'd4:begin
                // or 
                res = a|b;
                carry = 0;
            end
            3'd5:begin
                res = a^b;
                carry = 0;
            end
            3'd6:begin
                if(a[3]!=b[3]) begin
                    if(a[3]==1) res = 4'b1;
                    else res = 0;
                end
                else begin
                    if(a>b) res = 1;
                    else res = 0;
                end
                carry = 0;
            end
            3'd7:begin
                if(a==b) res = 1;
                else res =0;
                carry = 0;
            end
            default:begin
                carry = 0;
            end
        endcase
        overflow = (a[3]==b_minus[3])&&(res[3]!=a[3]);
        zero = ~(|res);
    end
endmodule