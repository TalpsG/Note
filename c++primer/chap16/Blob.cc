#include "Blob.hh"
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
template <typename T>
void Blob<T>::check(Blob<T>::size_type i, const std::string &msg) const {
  if (i >= data->size())
    throw std::out_of_range(msg);
}

template <typename T> T &Blob<T>::back() {
  check(0, "back on empty blob");
  return data->back();
}
template <typename T> T &Blob<T>::operator[](Blob<T>::size_type i) {
  check(i, "back on empty blob");
  return (*data)[i];
}
template <typename T> void Blob<T>::pop_back() {
  check(0, "pop_back on empty blob");
  data->pop_back();
}
template <typename T>
Blob<T>::Blob() : data(std::make_shared<std::vector<T>>()) {}
template <typename T>
Blob<T>::Blob(std::initializer_list<T> il)
    : data(std::make_shared<std::vector<T>>(il)) {}

template <typename T> BlobPtr<T> &BlobPtr<T>::operator++() {
  auto p = check(curr + 1, "fore increment");
  return (*p)[++curr];
}
template <typename T> BlobPtr<T> BlobPtr<T>::operator++(int) {
  BlobPtr ret = *this;
  ++*this;
  return ret;
}
template <typename T> BlobPtr<T> &BlobPtr<T>::operator--() {
  auto p = check(curr - 1, "fore increment");
  return (*p)[--curr];
}
template <typename T> BlobPtr<T> BlobPtr<T>::operator--(int) {
  BlobPtr ret = *this;
  --*this;
  return ret;
}
template <typename T> bool operator==(const Blob<T> &b1, const Blob<T> &b2) {
  return b1.data == b2.data;
}
template <typename T>
template <typename It>
Blob<T>::Blob(It beg, It end) : data(new std::vector<T>(beg, end)) {}

int main(int argc, char *argv[]) {
  std::vector<int> vec(10, 1);
  Blob<int> B(vec.begin(), vec.end());
  return 0;
}
