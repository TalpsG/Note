#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
class StrVec {
public:
  StrVec() : elements(nullptr), first_free(nullptr), cap(nullptr) {}
  StrVec(const StrVec &);
  StrVec(StrVec &&) noexcept;
  StrVec(std::initializer_list<std::string> &);
  StrVec &operator=(const StrVec &);
  StrVec &operator=(const StrVec &&);
  ~StrVec();
  void push_back(const std::string &);
  void push_back(std::string &&);
  std::size_t size() const { return first_free - elements; }
  std::size_t capacity() const { return cap - elements; };
  std::string *begin() const { return elements; }
  std::string *end() const { return first_free; }
  void reserve(std::size_t);
  void resize(std::size_t, const std::string &);

private:
  static std::allocator<std::string> alloc;
  void chk_n_alloc() {
    if (size() == capacity())
      realloc();
  }
  std::pair<std::string *, std::string *> alloc_n_copy(const std::string *,
                                                       const std::string *);
  void free();
  void realloc();
  std::string *elements;
  std::string *first_free;
  std::string *cap;
};
