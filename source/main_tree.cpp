#include <iostream>

#include "io_wrap.hpp"
#include "rbtree.hpp"

bool comparator(int a, int b) {
  if (a < b)
    return true;
  return false;
}

int main() {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, false);
  try {
    char cmd;
    while (IOWrap::GetFromInput(cmd, std::cin)) {
      if (cmd == 'k') {
        int x = 0;
        if (!(IOWrap::GetFromInput(x, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'k' ");
        }
        rbtree.insert(x);
      } else if (cmd == 'q') {
        int l = 0, r = 0;
        if (!(IOWrap::GetFromInput(l, std::cin) && IOWrap::GetFromInput(r, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'q' ");
        }
        std::cout << rbtree.rangeQuery(l, r) << ' ';
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