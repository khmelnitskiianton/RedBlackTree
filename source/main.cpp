#include <iostream>
#include "io_wrap.hpp"
#include "rbtree.hpp"

bool comparator(int a, int b) {
  if (a < b)
    return true;
  return false; 
}

int main() {
  Tree::RBTree<int, bool (*)(int, int)> rbtree = Tree::RBTree<int, bool (*)(int, int)>(comparator, true);
  rbtree.printTree();
  rbtree.insert(1);
  rbtree.insert(4);
  rbtree.insert(5);
  rbtree.insert(0);
  rbtree.insert(2);
  rbtree.insert(10);
  rbtree.insert(3);
  rbtree.printTree();
  rbtree.insert(20);
  rbtree.insert(18);
  rbtree.insert(21);
  rbtree.insert(23);
  rbtree.insert(7);
  rbtree.printTree();
  rbtree.inorderTreeWalk();
  return 0;
}