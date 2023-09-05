#include<verilated.h>
#include<stdio.h>
#include<nvboard.h>
#include<Vdriver.h>
#include<verilated_vcd_c.h>
unsigned int t=0;
void nvboard_bind_all_pins(Vdriver* top); 
Vdriver driver;
VerilatedVcdC* trace = new VerilatedVcdC;
void single_clk(){
    driver.clk=0;
    driver.eval();
    trace->dump(t);
    t++;
    driver.clk=1;
    driver.eval();
    trace->dump(t);
    t++;
}
void reset(){
    driver.clrn=0;
    for (int i = 0; i < 10; i++)
    {
        driver.eval();
    }
    driver.clrn=1;
}
int main(){
    nvboard_bind_all_pins(&driver);
    nvboard_init();
    Verilated::traceEverOn(true);
    reset();
    driver.trace(trace,5);
    trace->open("waveform.vcd");
    while(1){
        single_clk();
        nvboard_update();
    }
    return 0;

}
