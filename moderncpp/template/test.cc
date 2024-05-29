#include <concepts>
template <typename T>
concept Addable = requires(T a, T b) {
  { a + b } -> std::convertible_to<T>;
};

int main(int argc, char *argv[]) { return 0; }
