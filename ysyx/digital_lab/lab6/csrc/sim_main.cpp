#include<verilated.h>
#include<stdio.h>
#include<Vshift.h>
#include<stdio.h>
Vshift shift;
void single_clk(){
    shift.clk = 0;
    shift.eval();
    shift.clk = 1;
    shift.eval();
}
int main(){
    shift.mod = 5;
    for(int i=0;i<8;i++){
        shift.in=rand()&0x1;
        single_clk();
    }
    printf("%x\n",shift.data);
    unsigned char mod = 0;
    scanf("%hhd",&mod);
    shift.mod = mod;
    while(1){
        scanf("%hhd",&shift.in);
        single_clk();
        printf("%x\n",shift.data);
    }

    return 0;

}
