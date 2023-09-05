module top(
	input [7:0] in,
	input enable,
	output reg have_input,
	output reg [3:0] out,
	output reg [7:0] num);
	bcd7reg led(.num(out),.seg(num));
	assign have_input = |in;
	integer i;

	always @(in or  enable) begin
		if(enable) begin
			for(i=0;i<8;i=i+1) begin
				if(in[i]==1) begin
					out=i[3:0];
				end
				else out=out;
			end
		end
		else out = 4'b0;
	end
endmodule
		

