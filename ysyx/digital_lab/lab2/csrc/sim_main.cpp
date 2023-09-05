#include <nvboard.h>
#include <Vtop.h>

static Vtop dut;

void nvboard_bind_all_pins(Vtop* top);

int main() {
  nvboard_bind_all_pins(&dut);
  nvboard_init();
  while(1) {
    nvboard_update();
    printf("input: %x , enable= %x , out: %x , seg:%x\n",dut.in,dut.enable,dut.out,dut.num);
    dut.eval();
  }
}

