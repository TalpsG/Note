#include "strvec.hpp"
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

std::allocator<std::string> StrVec::alloc;
void StrVec::push_back(const std::string &str) {
  chk_n_alloc();
  alloc.construct(first_free++, str);
}
void StrVec::push_back(std::string &&str) {
  chk_n_alloc();
  alloc.construct(first_free++, std::move(str));
}
std::pair<std::string *, std::string *>
StrVec::alloc_n_copy(const std::string *b, const std::string *e) {
  auto data = alloc.allocate(e - b);
  return {data, std::uninitialized_copy(b, e, data)};
}

void StrVec::free() {
  if (elements) {
    // for each + lambda version
    std::for_each(elements, first_free,
                  [this](std::string &p) -> void { alloc.destroy(&p); });
    // for (auto p = first_free; p != elements;) {
    //   alloc.destroy(--p);
    // }

    alloc.deallocate(elements, cap - elements);
  }
}

StrVec::StrVec(const StrVec &sv) {
  auto newdata = alloc_n_copy(sv.begin(), sv.end());
  elements = newdata.first;
  first_free = cap = newdata.second;
}
StrVec::StrVec(StrVec &&sv) noexcept
    : elements(sv.elements), first_free(sv.first_free), cap(sv.cap) {
  sv.elements = sv.first_free = sv.cap = nullptr;
}
StrVec::StrVec(std::initializer_list<std::string> &ls) {
  auto newdata = alloc.allocate(ls.size());
  elements = newdata;
  first_free = cap = elements + ls.size();
  for (auto &i : ls) {
    alloc.construct(newdata++, std::move(i));
  }
}
StrVec::~StrVec() { free(); }
StrVec &StrVec::operator=(const StrVec &sv) {
  auto data = alloc_n_copy(sv.begin(), sv.end());
  free();
  elements = data.first;
  first_free = cap = data.second;
  return *this;
}
StrVec &StrVec::operator=(std::initializer_list<std::string> &ls) {
  auto data = alloc_n_copy(ls.begin(), ls.end());
  free();
  elements = data.first;
  first_free = cap = data.second;
  return *this;
}
std::string &StrVec::operator[](std::size_t n) { return elements[n]; }
StrVec &StrVec::operator=(const StrVec &&sv) {
  if (this == &sv)
    return *this;
  free();
  elements = sv.elements;
  first_free = sv.first_free;
  cap = sv.cap;
  return *this;
}
void StrVec::realloc() {
  auto newcap = (size() == 0 ? 10 : (size() * 2)) * 2;
  auto first = alloc.allocate(newcap);
  auto last = std::uninitialized_copy(std::make_move_iterator(begin()),
                                      std::make_move_iterator(end()), first);
  free();
  elements = first;
  first_free = last;
  cap = elements + newcap;
}
void StrVec::reserve(std::size_t t) {
  if (t > capacity()) {
    auto newcap = t;
    auto newdata = alloc.allocate(newcap);
    auto dest = newdata;
    auto src = elements;
    for (std::size_t i = 0; i < size(); ++i) {
      alloc.construct(dest++, std::move(*(src++)));
    }
    free();
    elements = newdata;
    first_free = dest;
    cap = elements + newcap;
  }
}
void StrVec::resize(std::size_t t, const std::string &str = "") {
  // 分成缩小或扩大两个分支处理
  if (t > size()) {
    // 大于容量需要扩容
    if (t > capacity()) {
      reserve(t);
    }
    // 初始化新的元素
    for (auto p = first_free; p != (elements + t); ++p) {
      alloc.construct(p, std::move(str));
    }
  } else {
    auto p = first_free;
    for (std::size_t i = 0; i < (t - size()); i++) {
      alloc.destroy(--p);
    }
  }
}
