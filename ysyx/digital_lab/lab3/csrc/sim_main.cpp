#include<verilated.h>
#include<stdio.h>
#include<Valu.h>
int main(){
    Valu* alu = new Valu;
    unsigned int ans,res,overflow,carry,zero;
    for(unsigned int i=3;i<8;i++){
        alu->mod = i;
        for(unsigned int j=0;j<16;j++){
            alu->a = j;
            for(unsigned int k=0;k<16;k++){
                alu->b = k;
                unsigned int com ;
                if(i == 1) com = ~k;
                else com = k;
                alu->eval();

                switch(i){
                    case 0:{
                        res = j+com;
                        ans = res%16;
                        carry = (res&0x16)>>4;
                        break;
                    }
                    //加法通过
                    case 1:{
                        res = j+com+1;
                        ans = res%16;
                        carry = (res&0x16)>>4;
                        break;
                    }
                    //pass
                    case 2:{
                        ans = (~j)&0xf;
                        carry = 0;
                        break;
                    }
                    //pass
                    case 3:{
                        ans = j&k;
                        carry = 0;
                        break;
                    }
                    case 4:{
                        ans = j|k;
                        carry = 0;
                        break;
                    }
                    case 5:{
                        ans = j^k;
                        carry = 0;
                        break;
                    }
                    case 6:{
                        if((j&0x80)==(k&0x80)){
                            //same sign
                            if((j&0x80)==1){
                                //both negative
                                if(j>=k) ans = 0;
                                else ans =1;
                            }
                            else{
                                //both pos
                                ans = j>k?1:0;
                            }
                        }
                        else{
                            //different sign
                            if(j&0x80){
                                ans = 0;
                            }
                            else ans = 1;
                        }
                        break;
                    }
                    case 7:{
                        if(j==k) ans= 1;
                        else ans = 0;
                        carry = 0;
                        break;
                    }
                }
                overflow = ((j&0x8)==(com&0x8))&&((ans&0x8)!=(j&0x8));
                zero = (ans==0)?1:0;
                if((alu->carry != carry )||(alu->overflow!=overflow)||(alu->res!=ans)||(alu->zero!=zero)){
                    printf("mod:%x i:%x\n",alu->mod,i);
                    printf("alu  a:%x,b:%x,carry:%x,overflow:%x,zero:%x,res:%x\n",alu->a,alu->b,alu->carry,alu->overflow,alu->zero,alu->res);
                    printf("for  j:%x,k:%x,carry:%x,overflow:%x,zero:%x,ans:%x\n\n",j,k,carry,overflow,zero,ans);
                }
                //printf("mod:%x i:%x\n",alu->mod,i);
                //printf("alu  a:%x,b:%x,carry:%x,overflow:%x,zero:%x,res:%x\n",alu->a,alu->b,alu->carry,alu->overflow,alu->zero,alu->res);
                //printf("for  j:%x,k:%x,carry:%x,overflow:%x,zero:%x,ans:%x\n\n",j,k,carry,overflow,zero,ans);
            }
        }
    }
    delete alu;
    return 0;

}
