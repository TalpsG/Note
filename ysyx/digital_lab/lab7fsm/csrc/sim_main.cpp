#include<verilated.h>
#include<stdio.h>
#include<Vfsm.h>
#include<stdio.h>
Vfsm fsm;

void single_clk(){
    fsm.clk = 0;
    fsm.eval();
    fsm.clk = 1;
    fsm.eval();
}
int main(){
    fsm.reset=1;
    single_clk();
    printf("out:%x\n",fsm.out);
    fsm.reset=0;
    while(1){
        scanf("%hhd",&fsm.in);
        single_clk();
        printf("out:%x\n",fsm.out);
    }
    return 0;

}
