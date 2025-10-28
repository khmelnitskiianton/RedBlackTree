#include <gtest/gtest.h>
#include "rbtree.hpp"

bool comparator(int a, int b) {
  if (a < b)
    return true;
  return false;
}

TEST(RBTreeTests, insertTest) {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, true);
  rbtree.insert(1);
  rbtree.insert(4);
  rbtree.insert(5);
  rbtree.insert(0);
  ASSERT_EQ(rbtree.contains(1), true);
  ASSERT_EQ(rbtree.contains(4), true);
  ASSERT_EQ(rbtree.contains(5), true);
  ASSERT_EQ(rbtree.contains(0), true);
  rbtree.erase(0);
  ASSERT_EQ(rbtree.contains(0), false);
}

TEST(RBTreeTests, eraseTest) {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, true);
  rbtree.insert(1);
  rbtree.insert(4);
  rbtree.insert(5);
  rbtree.insert(0);
  ASSERT_EQ(rbtree.contains(1), true);
  ASSERT_EQ(rbtree.contains(4), true);
  ASSERT_EQ(rbtree.contains(5), true);
  ASSERT_EQ(rbtree.contains(0), true);
  ASSERT_EQ(rbtree.erase(0), 1);
  ASSERT_EQ(rbtree.contains(0), false);
  ASSERT_EQ(rbtree.erase(0), 0);
  ASSERT_EQ(rbtree.erase(9), 0);
}

TEST(RBTreeTests, rangeQueryTest) {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, true);
  rbtree.insert(10);
  rbtree.insert(20);
  ASSERT_EQ(rbtree.rangeQuery(8, 31), 2);
  ASSERT_EQ(rbtree.rangeQuery(6, 9), 0);
  rbtree.insert(30);
  rbtree.insert(40);
  ASSERT_EQ(rbtree.rangeQuery(15, 40), 2);
}

TEST(RBTreeTests, initTest) {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, true);
  rbtree.insert(10);
  rbtree.insert(20);
  ASSERT_EQ(rbtree.rangeQuery(8, 31), 2);
  ASSERT_EQ(rbtree.rangeQuery(6, 9), 0);
  rbtree.insert(30);
  rbtree.insert(40);
  ASSERT_EQ(rbtree.rangeQuery(15, 40), 2);
  // Init
  Tree::RBTree<int, bool (*)(int, int)> new_rbtree = rbtree;
  ASSERT_EQ(new_rbtree.rangeQuery(8, 31), 3);
  ASSERT_EQ(new_rbtree.rangeQuery(6, 9), 0);
  ASSERT_EQ(new_rbtree.rangeQuery(15, 40), 2);
}

TEST(RBTreeTests, copyTest) {
  Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, true);
  rbtree.insert(10);
  rbtree.insert(20);
  ASSERT_EQ(rbtree.rangeQuery(8, 31), 2);
  ASSERT_EQ(rbtree.rangeQuery(6, 9), 0);
  rbtree.insert(30);
  rbtree.insert(40);
  ASSERT_EQ(rbtree.rangeQuery(15, 40), 2);
  // Copy
  Tree::RBTree<int, bool (*)(int, int)> new_rbtree(comparator, true);
  new_rbtree = rbtree;
  ASSERT_EQ(new_rbtree.rangeQuery(8, 31), 3);
  ASSERT_EQ(new_rbtree.rangeQuery(6, 9), 0);
  ASSERT_EQ(new_rbtree.rangeQuery(15, 40), 2);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}