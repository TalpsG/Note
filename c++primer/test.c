#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main() {
  double a = 0, b = 0, c = 0, f = 0;
  char d[100];
  scanf("%s", d);
  scanf("%lf", &c);
  if (strcmp(d, "现代") == 0)
    a = 14.0, b = 2.0;
  else if (strcmp(d, "桑塔纳") == 0)
    a = 12.0, b = 1.8;
  else if (strcmp(d, "富康") == 0)
    a = 10.0, b = 1.6;
  if (c <= 3)
    printf("%lf", a);
  else if (c > 3)
    f = a + b * (c - 3.0);
  printf("%lf", f);
  return 0;
}
