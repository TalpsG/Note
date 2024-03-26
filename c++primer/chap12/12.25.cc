int main(int argc, char *argv[]) {
  int *pa = new int[10];
  delete[] pa;
  *pa = 5;
  return 0;
}
