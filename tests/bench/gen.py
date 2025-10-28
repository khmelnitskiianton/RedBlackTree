import argparse
import random
import sys
from typing import List, Tuple

def gen_keys(n: int, lo: int, hi: int, rng: random.Random) -> List[int]:
    if hi - lo + 1 < n:
        raise ValueError(f"Диапазон [{lo}, {hi}] слишком мал для {n} уникальных ключей")
    # уникальные ключи
    return rng.sample(range(lo, hi + 1), k=n)

def gen_query(keys: List[int], lo: int, hi: int, rng: random.Random) -> Tuple[int, int]:
    """
    Диапазон «вокруг» случайного ключа:
    берём центр = случайный ключ, полуширина — случайная,
    затем обрезаем в [lo, hi].
    """
    center = rng.choice(keys)
    half = max(1, int((hi - lo) * (0.02 + 0.18 * rng.random())))  # ~2..20% диапазона
    a = max(lo, center - half)
    b = min(hi, center + half)
    # для [a,b) нужно гарантировать a<b
    # иногда сдвинем левую/правую границу чуть случайно
    a2 = rng.randint(a, max(a, b - 1))
    b2 = rng.randint(max(a2 + 1, a + 1), b)
    return a2, b2

def main():
  p = argparse.ArgumentParser(
    description="Генератор тестов: команды 'k <int>' и 'q <int> <int>'"
  )    
  p.add_argument("--keys", type=int,           default=1000000, help="число ключей (k)")
  p.add_argument("--queries", type=int,        default=1000000, help="число запросов (q)")
  p.add_argument("--min", dest="lo", type=int, default=0, help="минимальное значение ключа/границы")
  p.add_argument("--max", dest="hi", type=int, default=10**9, help="максимальное значение ключа/границы")
  p.add_argument("--seed", type=int, default=None, help="seed для воспроизводимости")
  p.add_argument("--out", type=str, default="-", help="файл для вывода или '-' для stdout")
  args = p.parse_args()

  rng = random.Random(args.seed)
  
  if args.lo >= args.hi:
    p.error("--min должен быть меньше --max")
  
  # генерим уникальные ключи
  keys = gen_keys(args.keys, args.lo, args.hi, rng)

  out_stream = sys.stdout if args.out == "-" else open(args.out, "w", encoding="utf-8")
  
  ops = ["k"] * args.keys + ["q"] * args.queries
  rng.shuffle(ops)
  ki = 0
  for op in ops:
      if op == "k":
          print(f"k {keys[ki]}", file=out_stream)
          ki += 1
      else:
          a, b = gen_query(keys, args.lo, args.hi, rng)
          print(f"q {a} {b}", file=out_stream)

if __name__ == "__main__":
  main()
