#include <iostream>
#include <iterator>
#include <set>

#ifdef TIME_COUNT
#include <chrono>
#endif

#include "io_wrap.hpp"

template <typename C, typename T> size_t range_query(const C &s, const T &fst, const T &snd) {
  using itt = typename C::const_iterator;
  if (snd <= fst)
    return 0;
  itt first = s.lower_bound(fst);
  itt last = s.upper_bound(snd);
  return std::distance(first, last); // std::distance для set
}

int main() {
  std::set<int> key_set;

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

        key_set.insert(x);

#ifdef TIME_COUNT
        auto end_insert = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end_insert - start_insert;
        total_insert += elapsed.count();
#endif

      } else if (cmd == 'q') {
        int l = 0, r = 0;
        if (!(IOWrap::GetFromInput(l, std::cin) && IOWrap::GetFromInput(r, std::cin))) {
          throw std::ios_base::failure("invalid args in request 'q' ");
        }

#ifdef TIME_COUNT
        auto start_rq = std::chrono::steady_clock::now();
#endif

        size_t rq = range_query<std::set<int>, int>(key_set, l, r);

#ifdef TIME_COUNT
        (void)rq; // avoid unused warning
        auto end_rq = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end_rq - start_rq;
        total_rq += elapsed.count();
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