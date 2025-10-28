#include <iostream>
#include <iterator>
#include <set>

#include "io_wrap.hpp"

template <typename C, typename T> size_t range_query(const C &s, const T &fst, const T &snd) {
  using itt = typename C::const_iterator;
  if (snd <= fst)
    return 0;
  itt first = s.lower_bound(fst);
  if (first == s.end())
    return 0;
  if (*first >= snd)
    return 0;
  itt last = s.lower_bound(snd);
  return std::distance(first, last); // std::distance для set
}

int main() {
  std::set<int> key_set;
  try {
    char cmd;
    while (IOWrap::GetFromInput(cmd, std::cin)) {
      if (cmd == 'k') {
        int x = 0;
        if (!(IOWrap::GetFromInput(x, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'k' ");
        }
        key_set.insert(x);
      } else if (cmd == 'q') {
        int l = 0, r = 0;
        if (!(IOWrap::GetFromInput(l, std::cin) && IOWrap::GetFromInput(r, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'q' ");
        }
        std::cout << range_query<std::set<int>, int>(key_set, l, r) << ' ';
      } else {
        throw std::ios_base::failure("unknown request in input data");
      }
    }
  } catch (const std::ios_base::failure &e) {
    std::cerr << "Bad input in data: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return 0;
}