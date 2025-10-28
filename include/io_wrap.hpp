#ifndef INCLUDE_IO_WRAP_HPP
#define INCLUDE_IO_WRAP_HPP

#include <fstream>

namespace IOWrap {

template <typename T> bool GetFromInput(T &arg, std::istream &input) {
  auto old = input.exceptions();
  input.exceptions(std::ios::goodbit);
  if (input >> arg) {
    input.exceptions(old);
    return true;
  }
  input.exceptions(old);
  return false;
}

template <typename T> void TryOpenFile(std::ifstream &input, const T &arg) {
  input.exceptions(std::ifstream::failbit | std::ifstream::badbit); // enable cin throw exceptions.
  input.open(arg);
}

} // namespace IOWrap

#endif