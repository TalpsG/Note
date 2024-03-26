#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <utility>
template <typename T> class talps_shared_ptr {
public:
  talps_shared_ptr() : ptr(nullptr), use_count(new unsigned(0)) {}
  talps_shared_ptr(talps_shared_ptr &&t) noexcept
      : ptr(t.ptr), use_count(t.use_count) {
    t.use_count = nullptr;
    t.ptr = nullptr;
    std::cout << "move ctor " << t.info() << '\n';
  }
  talps_shared_ptr(const talps_shared_ptr &t)
      : ptr(t.ptr), use_count(t.use_count) {
    ++(*use_count);
    std::cout << "copy ctor " << t.info() << '\n';
  }
  talps_shared_ptr(T *p) : ptr(p), use_count(new unsigned(1)) {
    std::cout << "normal pointer to initialize\n";
  }

  talps_shared_ptr &operator=(const talps_shared_ptr &sp) {
    std::cout << this->info() << " = " << sp.info() << "\n";
    ++(*sp.use_count);
    --(*use_count);
    if (use_count == 0) {
      delete ptr;
      delete use_count;
    }
    ptr = sp.ptr;
    use_count = sp.use_count;
    return *this;
  }
  T &operator*() {
    std::cout << "def:" << info() << '\n';
    if (use_count == nullptr) {
      throw std::runtime_error("derefing shared_ptr is nullptr");
    }
    return *ptr;
  }
  talps_shared_ptr *operator->() {
    if (use_count == nullptr) {
      throw std::runtime_error("-> shared_ptr is nullptr");
    }
    return ptr;
  }
  ~talps_shared_ptr() {
    std::cout << "dtor :" << info() << "\n";
    if (use_count) {
      --(*use_count);
      if (*use_count == 0) {
        std::cout << " shared ptr use count == 0 \n";
        delete ptr;
        delete use_count;
      }
    }
  }
  std::string info() const {
    std::stringstream ss;
    ss << ptr;
    std::string temp = ss.str();
    return "talps_shared_ptr info:" + temp + " : " +
           (use_count == nullptr ? "nullptr" : std::to_string(*use_count)) +
           " | ";
  }

private:
  T *ptr;
  unsigned *use_count;
};
template <typename T, typename... Args>
talps_shared_ptr<T> talps_make_shared(Args... rest) {
  return talps_shared_ptr<T>(new T(std::forward<Args>(rest)...));
}
int main(int argc, char *argv[]) {
  talps_shared_ptr<std::string> sp = talps_make_shared<std::string>("wwt");
  std::cout << *sp << "\n";

  return 0;
}
