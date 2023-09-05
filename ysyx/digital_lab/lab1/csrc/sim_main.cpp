#include "verilated.h"
#include <Vtop.h>
#include <nvboard.h>
static Vtop top;
void nvboard_bind_all_pins(Vtop* top);
int main(int argc,char **argv){
    nvboard_bind_all_pins(&top);
    nvboard_init();
    while(1){
        nvboard_update();
        top.eval();
    }
    

}