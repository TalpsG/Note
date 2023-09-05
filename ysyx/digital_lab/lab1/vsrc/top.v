module top(
  input [1:0] select,a,b,c,d,
  output [1:0] out
);
  MuxKeyWithDefault #(4, 2,2) i1 (out,select,2'b00,{2'b00,a,2'b01,b,2'b10,c,2'b11,d});
endmodule
