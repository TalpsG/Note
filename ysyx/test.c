#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void test(){
	char *p=strtok(NULL," ");
	printf("%s\n",p);
}
int main(){
	char p[]="123 456 789";
	char *s;
	s  = strtok(p," ");
	printf("%s\n",s);
	s  = strtok(NULL," ");
	printf("%s\n",s);
	test();


	return 0;
}
