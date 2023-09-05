module driver(
    input wire clk,clrn,ps2_clk,ps2_data,
    output reg [7:0] data,
    output reg [7:0] firstseg,secondseg,thirdseg,forthseg,fifthseg,sixthseg
);
    reg [3:0] first,second,third,forth;

    reg [7:0] firstseg_real,secondseg_real,thirdseg_real,forthseg_real;

    reg [3:0] lsb,msb;
    reg [7:0] ascii;
    reg [1:0] w;


    assign {second,first} = data;


    // 键盘通码
    bcd7reg firstbit(first,firstseg_real); 
    bcd7reg secondbit(second,secondseg_real);

    //对应的ascii
    bcd7reg thirdbit(third,thirdseg_real);
    bcd7reg forthbit(forth,forthseg_real);



    //按键次数
    bcd7reg fifthbit(lsb,fifthseg);
    bcd7reg sixthbit(msb,sixthseg);
    reg nextdata_n;
    reg ready;
    reg overflow;
    ps2_keyboard keyboard(.clk(clk),.clrn(clrn),.ps2_clk(ps2_clk),.ps2_data(ps2_data),.nextdata_n(nextdata_n),.data(data),.ready(ready),.overflow(overflow));

    initial begin
        lsb = 0;
        msb = 0;
        w = 1;
        firstseg = 8'hff;
        secondseg= 8'hff;
        thirdseg= 8'hff;
        forthseg= 8'hff;
    end


    always@(posedge clk) begin 
        if(ready) begin
            nextdata_n <=0;
            if(data==8'hf0) begin
                w<=w+1;
            end
            else if(w==1)begin
                w<=w+1;
            end
            else if(w==2)begin
                w<=0;
            end
        end
        else begin
            nextdata_n <=1;
        end
    end
    always@(posedge ready)begin
        if(data==8'hf0) begin
            if(lsb==4'h9) begin
                if(msb==4'h9) msb<=0;
                else msb<=msb+1;
                lsb <= 0;
            end
            else lsb<=lsb+1;
        end
    end
    always@(*)begin
        case(data)
            8'd22:ascii=49;
            8'd30:ascii=50;// { 2 }
            8'd38:ascii=51;// { 3 }
            8'd37:ascii=52;// { 4 }
            8'd46:ascii=53;// { 5 }
            8'd54:ascii=54;// { 6 }
            8'd61:ascii=55;// { 7 }
            8'd62:ascii=56;// { 8 }
            8'd70:ascii=57;// { 9 }
            8'd69:ascii=48;// { 0 }
            8'd28:ascii=97;// { a }
            8'd50:ascii=98;// { b }
            8'd33:ascii=99;// { c }
            8'd35:ascii=100;// { d }
            8'd36:ascii=101;// { e }
            8'd43:ascii=102;// { f }
            8'd52:ascii=103;// { g }
            8'd51:ascii=104;// { h }
            8'd67:ascii=105;// { i }
            8'd59:ascii=106;// { j }
            8'd66:ascii=107;// { k }
            8'd75:ascii=108;// { l }
            8'd58:ascii=109;// { m }
            8'd49:ascii=110;// { n }
            8'd68:ascii=111;// { o }
            8'd77:ascii=112;// { p }
            8'd21:ascii=113;// { q }
            8'd45:ascii=114;// { r }
            8'd27:ascii=115;// { s }
            8'd44:ascii=116;// { t }
            8'd60:ascii=117;// { u }
            8'd42:ascii=118;// { v }
            8'd29:ascii=119;// { w }
            8'd34:ascii=120;// { x }
            8'd53:ascii=121;// { y }
            8'd26:ascii=122;// { z }
            default:ascii=8'h0;
        endcase
        /* verilator lint_off WIDTHTRUNC */
        third = ascii%16;
        forth = ascii>>4;
        /* verilator lint_off WIDTHTRUNC */
    end
    always@(posedge ready)begin
        if(w == 0)begin
            firstseg = firstseg_real;
            secondseg = secondseg_real;
            thirdseg = thirdseg_real;
            forthseg = forthseg_real;
        end
        else begin
            firstseg = 8'hff;
            secondseg = 8'hff;
            thirdseg = 8'hff;
            forthseg = 8'hff;
        end

    end


endmodule 
