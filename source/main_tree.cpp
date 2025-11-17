#include <cstddef>
#include <iostream>

#include "io_wrap.hpp"
#include "rbtree.hpp"

#ifdef TIME_COUNT
  #include <chrono>
#endif

bool comparator(int a, int b) {
  if (a < b)
    return true;
  return false;
}

int main() {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator);

#ifdef TIME_COUNT
  double total_insert = 0.0;
  double total_rq = 0.0;
  double total_time = 0.0;
  auto start_time = std::chrono::steady_clock::now(); 
#endif

  try {
    char cmd;
    while (IOWrap::GetFromInput(cmd, std::cin)) {
      if (cmd == 'k') {
        int x = 0;
        if (!(IOWrap::GetFromInput(x, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'k' ");
        }

#ifdef TIME_COUNT
        auto start_insert = std::chrono::steady_clock::now(); 
#endif

        rbtree.insert(x);

#ifdef TIME_COUNT
        auto end_insert = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_insert = end_insert - start_insert;
        total_insert += elapsed_insert.count();
#endif  

      } else if (cmd == 'q') {
        int l = 0, r = 0;
        if (!(IOWrap::GetFromInput(l, std::cin) && IOWrap::GetFromInput(r, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'q' ");
        }

#ifdef TIME_COUNT
        auto start_rq = std::chrono::steady_clock::now(); 
#endif

        size_t rq = rbtree.rangeQuery(l, r);

#ifdef TIME_COUNT
        (void) rq; // avoid unused warning
        auto end_rq = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_rq = end_rq - start_rq;
        total_rq += elapsed_rq.count();
#else
        std::cout << rq << ' ';
#endif  

      } else {
        throw std::ios_base::failure("unknown request in input data");
      }
    }
  } catch (const std::ios_base::failure &e) {
    std::cerr << "Bad input in data: " << e.what() << '\n';
    return EXIT_FAILURE;
  }

#ifdef TIME_COUNT
  auto end_time = std::chrono::steady_clock::now();
  total_time = std::chrono::duration<double>(end_time - start_time).count();
  std::cout << "Total insert time (in s): " << total_insert << '\n';
  std::cout << "Total rq     time (in s): " << total_rq << '\n';
  std::cout << "Total run    time (in s): " << total_time << '\n';
#endif

  return 0;
}