#ifndef INCLUDE_RBTREE_HPP
#define INCLUDE_RBTREE_HPP

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <format>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace Tree {
/// Requires bool comparator_(KeyT a, KeyT b)
/// If a < b gives true
/// else == (a <= b) gives false
template <typename KeyT, typename Comp> class RBTree {
    enum class Color { Red = 0, Black = 1 };
    class Node {
        KeyT key_;
        Node *parent_ = nullptr;
        Node *left_ = nullptr;
        Node *right_ = nullptr;
        Color color_ = Color::Black;
        size_t size_ = 1;

      public:
        Node() = default;
        Node(const Node &) = default;
        Node &operator=(const Node &) = default;

        Node(const KeyT &key) : key_(key) {}

        const KeyT &getKey() const { return key_; }

        inline void setParent(Node *parent) { parent_ = parent; }
        inline void setLeft(Node *left) { left_ = left; }
        inline void setRight(Node *right) { right_ = right; }
        inline void setColor(Color color) { color_ = color; }
        inline void setSize(size_t size) { size_ = size; }

        inline Node *getParent() const { return parent_; }
        inline Node *getLeft() const { return left_; }
        inline Node *getRight() const { return right_; }
        inline Color getColor() const { return color_; }
        inline size_t getSize() const { return size_; }

        inline bool isRed() const { return color_ == Color::Red; }
        inline bool isBlack() const { return color_ == Color::Black; }
    };
    using SrcDstPair = std::pair<const Node *, Node *>;

    Node nil_node_;           // Nill object that points to itself
    Node *nil_ = &nil_node_;  // pointer of nill
    Node *root_ = &nil_node_; // root
    Comp comparator_;         // comparator

  private:
    void copyTree(const RBTree &rhs) {
      if (this == &rhs)
        return;

      // Free previous tree if it was not empty
      freeTree();
      // Close previous logs
      closeLogger();

      // Copy logger
      copyLogger(rhs);

      // Src to dst values coping
      nil_node_.setColor(Color::Black);
      nil_node_.setLeft(&nil_node_);
      nil_node_.setRight(&nil_node_);
      nil_node_.setParent(&nil_node_);
      nil_node_.setSize(0);
      nil_ = &nil_node_;
      root_ = &nil_node_;
      comparator_ = rhs.comparator_;

      if (rhs.root_ == rhs.nil_)
        return;

      Node *new_root = new Node(rhs.root_->getKey());
      new_root->setColor(Color::Black);
      new_root->setLeft(nil_);
      new_root->setRight(nil_);
      new_root->setParent(nil_);
      new_root->setSize(rhs.root_->getSize());
      root_ = new_root;

      std::stack<SrcDstPair> nodes_stack;
      nodes_stack.push({rhs.root_, new_root});
      while (!nodes_stack.empty()) {
        // Take pair of src-dst nodes
        SrcDstPair sdpair = nodes_stack.top();
        nodes_stack.pop();
        const Node *src = sdpair.first;
        Node *dst = sdpair.second;
        // Left child
        if (src->getLeft() != rhs.nil_) {
          Node *child = new Node(src->getLeft()->getKey());
          dst->setLeft(child);
          child->setColor(src->getLeft()->getColor());
          child->setLeft(nil_);
          child->setRight(nil_);
          child->setSize(src->getLeft()->getSize());
          child->setParent(dst);
          nodes_stack.push({src->getLeft(), child});
        } else {
          dst->setLeft(nil_);
        }
        // Right child
        if (src->getRight() != rhs.nil_) {
          Node *child = new Node(src->getRight()->getKey());
          dst->setRight(child);
          child->setColor(src->getRight()->getColor());
          child->setLeft(nil_);
          child->setRight(nil_);
          child->setSize(src->getRight()->getSize());
          child->setParent(dst);
          nodes_stack.push({src->getRight(), child});
        } else {
          dst->setRight(nil_);
        }
      }
    }

    /// Get size of node
    inline int nodeSize(const Node *node) const { return (node == nil_) ? 0 : node->getSize(); }

    /// Update one size with its l/r nodes sizes
    inline void updateSize(Node *node) {
      if (node == nil_)
        return;
      node->setSize(1 + nodeSize(node->getLeft()) + nodeSize(node->getRight()));
    }

    /// Update sizes for all branch
    inline void updateSizeUp(Node *node) {
      while (node != nil_) {
        updateSize(node);
        node = node->getParent();
      }
    }

    /// Method returns an ptr to the first element that compares not less than x.
    inline Node *lowerBound(const KeyT &key) const { return lowerBound(root_, key); }
    inline Node *lowerBound(Node *node, const KeyT &key) const {
      Node *save_node = nil_;
      while (node != nil_) {
        if (!comparator_(node->getKey(), key)) {
          save_node = node;
          node = node->getLeft();
        } else {
          node = node->getRight();
        }
      }
      return save_node;
    }

    /// Method returns an ptr to the first element that compares greater than x.
    inline Node *upperBound(const KeyT &key) const { return upperBound(root_, key); }
    inline Node *upperBound(Node *node, const KeyT &key) const {
      Node *save_node = nil_;
      while (node != nil_) {
        if (comparator_(key, node->getKey())) {
          save_node = node;
          node = node->getLeft();
        } else {
          node = node->getRight();
        }
      }
      return save_node;
    }

    /// Method counts amount of nodes that less than node
    inline int rankOfNode(const Node *p) const {
      if (p == nil_)
        return nodeSize(root_);
      int r = nodeSize(p->getLeft());
      const Node *cur = p;
      while (cur->getParent() != nil_) {
        const Node *par = cur->getParent();
        if (cur == par->getRight()) {
          r += 1 + nodeSize(par->getLeft());
        }
        cur = par;
      }
      return r;
    }

    /// Method counts amount of nodes that less than key
    inline int rankLowerBound(const KeyT &key) const {
      const Node *p = lowerBound(key);
      return rankOfNode(p);
    }

    /// Method counts amount of nodes that less or equal than key
    inline int rankUpperBound(const KeyT &key) const {
      const Node *p = upperBound(key);
      return rankOfNode(p);
    }

    /// Wrap for comparator to check equality
    inline bool compareEqual(const KeyT &a, const KeyT &b) const { return !comparator_(a, b) && !comparator_(b, a); }

    /// Method of free tree's nodes
    void freeTree() {
      if (root_ == nil_)
        return;

      std::stack<Node *> node_stack;
      node_stack.push(root_);
      while (!node_stack.empty()) {
        Node *delete_node = node_stack.top();
        node_stack.pop();

        if (delete_node->getLeft() != nil_)
          node_stack.push(delete_node->getLeft());
        if (delete_node->getRight() != nil_)
          node_stack.push(delete_node->getRight());

        delete delete_node;
      }
    }

    /// Method to search key in tree
    /// Return non-const node ptr
    inline Node *search(const KeyT &key) const { return search(root_, key); }
    /// Method to search key in tree's node using loop(not recursion)
    inline Node *search(Node *node, const KeyT &key) const {
      while ((node != nil_) && (!compareEqual(key, node->getKey()))) {
        if (comparator_(key, node->getKey()))
          node = node->getLeft();
        else
          node = node->getRight();
      }
      return node;
    }

    /// Method to walk tree's node recursively
    void inorderTreeWalk(const Node *node) const {
      if (node == nil_)
        return;
      inorderTreeWalk(node->getLeft());
      std::cout << node->getKey() << '\n';
      inorderTreeWalk(node->getRight());
    }

    /// Method finds minimum node in tree
    inline Node *minimum() const { return minimum(root_); }
    /// Method finds minimum node from tree's node
    inline Node *minimum(Node *node) const {
      while (node->getLeft() != nil_)
        node = node->getLeft();
      return node;
    }
    /// Method finds maximum node in tree
    inline const Node *maximum() const { return maximum(root_); }
    /// Method finds maximum node from tree's node
    inline const Node *maximum(const Node *node) const {
      while (node->getRight() != nil_)
        node = node->getRight();
      return node;
    }

    /// Method of find "next" node from given
    /// If maximum - return NIL
    inline const Node *successor(const Node *node) const {
      if (node->getRight() != nil_)
        return minimum(node->getRight());

      Node *save_node = node->getParent();
      while ((save_node != nil_) && (node == save_node->getRight())) {
        node = save_node;
        save_node = save_node->getParent();
      }
      return save_node;
    }
    /// Method of find "previous" node from given
    /// If minimum - return NIL
    inline const Node *predecessor(const Node *node) const {
      if (node->getLeft() != nil_)
        return maximum(node->getLeft());

      Node *save_node = node->getParent();
      while ((save_node != nil_) && (node == save_node->getLeft())) {
        node = save_node;
        save_node = save_node->getParent();
      }
      return save_node;
    }

    /// Method rotates node for left
    /// Requires right leaf != NIL
    inline void rotateLeft(Node *x) {
      Node *y = x->getRight();   // set y
      x->setRight(y->getLeft()); // Make y left subtree in right for x

      if (y->getLeft() != nil_) {
        y->getLeft()->setParent(x);
      }

      y->setParent(x->getParent()); // Move parent of x to y

      if (x->getParent() == nil_) {
        root_ = y;
      } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
      } else {
        x->getParent()->setRight(y);
      }

      y->setLeft(x); // Make x - left for y
      x->setParent(y);

      // Update sizes
      updateSize(x);
      updateSize(y);
    }

    /// Method rotates node for right
    /// Requires left leaf != NIL
    inline void rotateRight(Node *x) {
      Node *y = x->getLeft();    // set y
      x->setLeft(y->getRight()); // Make y right subtree in left for x

      if (y->getRight() != nil_) {
        y->getRight()->setParent(x);
      }

      y->setParent(x->getParent()); // Move parent of x to y

      if (x->getParent() == nil_) {
        root_ = y;
      } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
      } else {
        x->getParent()->setRight(y);
      }

      y->setRight(x); // Make x - right for y
      x->setParent(y);

      // Update sizes
      updateSize(x);
      updateSize(y);
    }

    inline void insertFixup(Node *z) {
      Node *y = nil_;
      while (z->getParent()->isRed()) {
        if (z->getParent() == z->getParent()->getParent()->getLeft()) {
          y = z->getParent()->getParent()->getRight();
          if (y->isRed()) {
            z->getParent()->setColor(Color::Black);
            y->setColor(Color::Black);
            z->getParent()->getParent()->setColor(Color::Red);
            z = z->getParent()->getParent();
          } else {
            if (z == z->getParent()->getRight()) {
              z = z->getParent();
              rotateLeft(z);
            }
            z->getParent()->setColor(Color::Black);
            z->getParent()->getParent()->setColor(Color::Red);
            rotateRight(z->getParent()->getParent());
          }
        } else {
          y = z->getParent()->getParent()->getLeft();
          if (y->isRed()) {
            z->getParent()->setColor(Color::Black);
            y->setColor(Color::Black);
            z->getParent()->getParent()->setColor(Color::Red);
            z = z->getParent()->getParent();
          } else {
            if (z == z->getParent()->getLeft()) {
              z = z->getParent();
              rotateRight(z);
            }
            z->getParent()->setColor(Color::Black);
            z->getParent()->getParent()->setColor(Color::Red);
            rotateLeft(z->getParent()->getParent());
          }
        }
      }
      root_->setColor(Color::Black);
    }

    inline void transplant(Node *u, Node *v) {
      if (u->getParent() == nil_) {
        root_ = v;
      } else if (u == u->getParent()->getLeft()) {
        u->getParent()->setLeft(v);
      } else {
        u->getParent()->setRight(v);
      }
      v->setParent(u->getParent());
    }

    inline void eraseFixup(Node *x) {
      Node *w = nil_;
      while ((x != root_) && (x->getColor() == Color::Black)) {
        if (x == x->getParent()->getLeft()) {
          // w is x's sibling
          w = x->getParent()->getRight();

          if (w->getColor() == Color::Red) {
            w->setColor(Color::Black);
            x->getParent()->setColor(Color::Red);
            rotateLeft(x->getParent());
            w = x->getParent()->getRight();
          }
          if ((w->getLeft()->getColor() == Color::Black) && (w->getRight()->getColor() == Color::Black)) {
            w->setColor(Color::Red);
            x = x->getParent();
          } else {
            if (w->getRight()->getColor() == Color::Black) {
              w->getLeft()->setColor(Color::Black);
              w->setColor(Color::Red);
              rotateRight(w);
              w = x->getParent()->getRight();
            }
            w->setColor(x->getParent()->getColor());
            x->getParent()->setColor(Color::Black);
            w->getRight()->setColor(Color::Black);
            rotateLeft(x->getParent());
            x = root_;
          }
        } else {
          // w is x's sibling
          w = x->getParent()->getLeft();

          if (w->getColor() == Color::Red) {
            w->setColor(Color::Black);
            x->getParent()->setColor(Color::Red);
            rotateRight(x->getParent());
            w = x->getParent()->getLeft();
          }
          if ((w->getRight()->getColor() == Color::Black) && (w->getLeft()->getColor() == Color::Black)) {
            w->setColor(Color::Red);
            x = x->getParent();
          } else {
            if (w->getLeft()->getColor() == Color::Black) {
              w->getRight()->setColor(Color::Black);
              w->setColor(Color::Red);
              rotateLeft(w);
              w = x->getParent()->getLeft();
            }
            w->setColor(x->getParent()->getColor());
            x->getParent()->setColor(Color::Black);
            w->getLeft()->setColor(Color::Black);
            rotateRight(x->getParent());
            x = root_;
          }
        }
      }
      x->setColor(Color::Black);
    }

    /// Method for deleting
    inline void erase(Node *z) {
      Node *y = z;
      Node *x = nil_;
      Color y_orig_color = y->getColor();
      if (z->getLeft() == nil_) {
        x = z->getRight();
        transplant(z, z->getRight()); // replace z by its right child
      } else if (z->getRight() == nil_) {
        x = z->getLeft();
        transplant(z, z->getLeft()); // replace z by its left child
      } else {
        y = minimum(z->getRight()); // y is z’s successor
        y_orig_color = y->getColor();
        x = y->getRight();
        if (y != z->getRight()) {
          transplant(y, y->getRight());
          y->setRight(z->getRight());  // z’s right child becomes
          y->getRight()->setParent(y); // y’s right child
        } else {
          x->setParent(y); // in case x is NIL
        }
        transplant(z, y);           // replace z by its successor y
        y->setLeft(z->getLeft());   // and give z’s left child to y,
        y->getLeft()->setParent(y); // which had no left child
        y->setColor(z->getColor());
      }
      // if any red-black violations occured, correct them
      if (y_orig_color == Color::Black) {
        eraseFixup(x);
      }

      // Update sizes
      Node *n = x;
      if (n == nil_)
        n = x->getParent(); // if nil - get to its parent
      updateSizeUp(n);

      // Revive nil_'s parent to nil_(changes was required by algorithm)
      // And keep root black
      nil_->setParent(nil_);
      root_->setColor(Color::Black);
      delete z;
    }

    std::pair<bool, int> checkNode(const Node *n) const {
      if (n == nil_)
        return {true, 1}; // NIL is black, contributes 1

      auto [l_ok, l_bh] = checkNode(n->getLeft());
      auto [r_ok, r_bh] = checkNode(n->getRight());

      // Strict BST property with comparator: left < n < right
      bool bst_ok = (n->getLeft() == nil_ || comparator_(n->getLeft()->getKey(), n->getKey())) &&
                    (n->getRight() == nil_ || comparator_(n->getKey(), n->getRight()->getKey()));

      // Red node must have black children
      bool red_ok = !(n->isRed() && (n->getLeft()->isRed() || n->getRight()->isRed()));

      // Same black height on both sides
      bool bh_ok = (l_bh == r_bh);

      int bh = l_bh + (n->isBlack() ? 1 : 0);
      return {l_ok && r_ok && bst_ok && red_ok && bh_ok, bh};
    }

    /// Method for verifing RB invariants
    bool checkInvariants() const {
      if (root_ == nil_)
        return true; // empty tree is valid
      if (root_->getColor() != Color::Black)
        return false; // root must be black
      return checkNode(root_).first;
    }

    /// Methods for use tree
  public:
    RBTree(Comp comparator, bool enable_log = false) : comparator_(comparator), enable_log_(enable_log) {
      nil_node_.setColor(Color::Black);
      nil_node_.setLeft(&nil_node_);
      nil_node_.setRight(&nil_node_);
      nil_node_.setParent(&nil_node_);
      nil_node_.setSize(0);

      printLogStart();
    }

    RBTree(const RBTree &rhs) { copyTree(rhs); }

    RBTree &operator=(const RBTree &rhs) {
      copyTree(rhs);
      return *this;
    }

    ~RBTree() {
      freeTree();
      printLogFinish();
    }

    bool contains(const KeyT &key) const { return search(root_, key) != nil_; }

    /// Method for insert key in rbtree
    void insert(const KeyT &key) {
      // If in tree dont add new, all uniq
      if (contains(key))
        return;

      Node *z = new Node(key);
      Node *y = nil_;
      Node *x = root_;
      while (x != nil_) {
        y = x;
        if (comparator_(z->getKey(), x->getKey()))
          x = x->getLeft();
        else
          x = x->getRight();
      }
      z->setParent(y);
      if (y == nil_)
        root_ = z;
      else if (comparator_(z->getKey(), y->getKey())) {
        y->setLeft(z);
      } else {
        y->setRight(z);
      }
      z->setLeft(nil_);
      z->setRight(nil_);
      z->setColor(Color::Red);
      insertFixup(z);

      // Update sizes
      updateSizeUp(z);

      // if (!checkInvariants())
      //   throw std::runtime_error("Bad Red Black tree, no invariants");
    }

    size_t erase(const KeyT &key) {
      Node *find_node = search(key);
      if (find_node == nil_)
        return 0;
      erase(find_node);

      // if (!checkInvariants())
      //   throw std::runtime_error("Bad Red Black tree, no invariants");

      return 1;
    }

    size_t rangeQuery(const KeyT &begin, const KeyT &end) {
      if (!comparator_(begin, end) && !compareEqual(begin, end))
        return 0;
      int r1 = rankLowerBound(begin);
      int r2 = rankLowerBound(end);
      // [first, second)
      return (r2 > r1) ? (r2 - r1) : 0;
    }

    /// Method to walk tree recursively
    void inorderTreeWalk() { inorderTreeWalk(root_); }

    /// Grpahviz log section
  private:
    static constexpr const char *FolderLogPath = "/tmp/rbtree_log/";
    static constexpr const char *FileLogName = "log.html";
    static constexpr const char *FileGraphName = "graph.dot";
    static constexpr const char *TypeOfImage = "svg";

    static constexpr const char *FillBackColor = "#FFFACD";
    static constexpr const char *FillBackGraph = "#FFA07A";
    static constexpr const char *FillBackGraphNull = "#BC8F8F";
    static constexpr const char *FillFrontColor = "#000000";
    static constexpr const char *ColorFrame = "#000000";
    static constexpr const char *ColorStipple = "#483D8B";
    static constexpr const char *ColorEdgeGraph = "#000000";
    static constexpr const char *ColorEdgeHead = "#000000";
    static constexpr const char *ColorRed = "#FF0000";
    static constexpr const char *ColorBlack = "#808080";

    bool enable_log_ = false;
    boost::filesystem::path log_dir;
    boost::filesystem::path graph_path;
    boost::filesystem::path log_path;

    std::ofstream file_log_;
    std::ofstream file_graph_;

  public:
    // Get path to dir with this tree logs
    std::string getLogDirPath() const { return log_dir.string(); }

    void printLogTree(const char *file, const char *function, const size_t line) {
      if (!enable_log_)
        return;

      GenerateGraph();

      // Run: dot <path to dot graph> -Tsvg

      // Args for dot
      std::vector<std::string> args{};
      args.emplace_back(graph_path.string());
      args.emplace_back("-T" + std::string(TypeOfImage));

      // Run command using boost::process
      boost::process::ipstream dot_out;
      // Need absolute path for 'dot'
      boost::filesystem::path graphviz_bin = boost::process::search_path("dot");
      if (graphviz_bin.empty()) {
        throw std::runtime_error("Graphviz 'dot' not found in PATH");
      }

      int exit_code = 0;
      try {
        exit_code = boost::process::system(graphviz_bin, boost::process::args(args), boost::process::std_out > dot_out);
      } catch (const boost::process::process_error &e) {
        // failed exec, ENOENT, EACCES, etc.
        throw std::runtime_error(std::string("Failed to start 'dot': ") + e.what());
      }

      file_log_ << "\n<p>\n<b><big> ### In file: " << file << ",\tIn function: " << function << ",\tIn line: " << line
                << " ### </big></b>\n";

      // If we’re embedding SVG inline, skip the first 6 header lines
      std::string linebuf;
      for (int i = 0; i < 6 && std::getline(dot_out, linebuf); ++i) {
        // discard header lines
      }
      // Copy the rest directly into the HTML log
      while (std::getline(dot_out, linebuf)) {
        file_log_ << linebuf << '\n';
      }

      if (exit_code != 0) {
        // dot failed
        throw std::runtime_error("dot failed with exit code " + std::to_string(exit_code));
      }

      file_log_ << "</p>\n<hr size=\"4\" color=\"#000000\">\n\n";
    }

  private:
    void copyLogger(const RBTree &rhs) {
      enable_log_ = rhs.enable_log_;

      // Dont copy logs from previous tree!
      printLogStart();
    }
    
    void closeLogger() {
      printLogFinish();
    }

    void printLogStart(void) {
      // If disable logging
      if (!enable_log_)
        return;

      log_dir =
          boost::filesystem::path(FolderLogPath) / boost::filesystem::path(std::format("{:p}", static_cast<const void *>(nil_)));
      bool need_init = !boost::filesystem::exists(log_dir) || !boost::filesystem::is_directory(log_dir);
      if (need_init) {
        boost::system::error_code ec;
        boost::filesystem::create_directories(log_dir, ec);
        if (ec)
          throw std::runtime_error("creating path for logs failed: " + ec.message());
      }
      log_path = log_dir / FileLogName;
      graph_path = log_dir / FileGraphName;
      file_log_.open(log_path.string(), std::ios::out | std::ios::trunc);

      file_log_ << "<!DOCTYPE html>\n"
                   "<html lang=\"eng\">\n"
                   "<head>\n"
                   "<meta charset = \"utf-8\"/>\n"
                   "<title>LOGS</title>\n"
                   "<style>\n"
                   "body {\n"
                   "\tbackground-color: "
                << FillBackColor
                << " ;\n"
                   "\tcolor: "
                << FillFrontColor
                << ";\n"
                   "}\n"
                   "</style>\n"
                   "</head>\n"
                   "<body>\n"
                   "<pre>\n"
                   "<header>\n"
                   "<h1> Logs for Binary Tree </h1>\n"
                   "</header>\n"
                   "<main>\n"
                   "<pre>\n";
    }
    void printLogFinish(void) {
      // If not logs
      if (!enable_log_)
        return;

      file_log_ << "</pre>\n"
                   "</main>\n"
                   "</body>\n"
                   "</html>\n";
      file_log_.close();
    }
    void GenerateGraph() {
      file_graph_.open(graph_path, std::ios::out | std::ios::trunc);

      file_graph_ << "digraph G{\n"
                     "rankdir = TB;\n"
                     "bgcolor = \""
                  << FillBackColor
                  << "\";\n"
                     "node[color = \""
                  << ColorFrame
                  << "\", fontsize = 10];\n"
                     "edge[color = \""
                  << ColorFrame
                  << "\", fontsize = 15];\n"
                     "subgraph cluster_list {\n"
                     "\tnode[style = filled, fontsize = 10];\n"
                     "\tedge[color = \""
                  << ColorFrame
                  << "\", fontsize = 15];\n"
                     "\tstyle = \"dashed\";\n"
                     "\tcolor = \""
                  << ColorStipple << "\";\n";

      WriteTree();

      file_graph_ << "}\n"; // close subgraph

      WriteRoot();

      WriteNil();

      file_graph_ << "}\n"; // close digraph

      file_graph_.close();
    }
    void WriteTree() {
      if (root_ == nil_) {
        return;
      }
      WriteNode(root_);
    }
    void WriteNode(Node *CurrentNode) {
      if (CurrentNode == nil_)
        return;
      if (CurrentNode->isBlack()) {
        file_graph_ << "\tnode" << CurrentNode << "[shape = Mrecord, style = filled, fillcolor = \"" << ColorBlack
                    << "\", color = \"" << ColorEdgeGraph << "\", label = \"{ PARENT: " << CurrentNode->getParent()
                    << " | PTR: " << CurrentNode << " | DATA: " << CurrentNode->getKey() << " | SIZE: " << CurrentNode->getSize()
                    << " | { <f0> LEFT: " << CurrentNode->getLeft() << " | <f1> RIGHT: " << CurrentNode->getRight()
                    << " }}\"];\n";
      }
      if (CurrentNode->isRed()) {
        file_graph_ << "\tnode" << CurrentNode << "[shape = Mrecord, style = filled, fillcolor = \"" << ColorRed
                    << "\", color = \"" << ColorEdgeGraph << "\", label = \"{ PARENT: " << CurrentNode->getParent()
                    << " | PTR: " << CurrentNode << " | DATA: " << CurrentNode->getKey() << " | SIZE: " << CurrentNode->getSize()
                    << " | { <f0> LEFT: " << CurrentNode->getLeft() << " | <f1> RIGHT: " << CurrentNode->getRight()
                    << " }}\"];\n";
      }
      if (CurrentNode->getLeft() != nil_) {
        WriteNode(CurrentNode->getLeft());
        file_graph_ << "\tnode" << CurrentNode << ": <f0> -> node" << CurrentNode->getLeft() << "[color = \"" << ColorEdgeGraph
                    << "\"]\n";
      }
      if (CurrentNode->getRight() != nil_) {
        WriteNode(CurrentNode->getRight());
        file_graph_ << "\tnode" << CurrentNode << ": <f1> -> node" << CurrentNode->getRight() << "[color = \"" << ColorEdgeGraph
                    << "\"]\n";
      }
    }
    void WriteRoot() {
      file_graph_ << "All[shape = Mrecord, label = \" ROOT | <f0> " << root_ << " \", style = \"filled\", fillcolor = \""
                  << FillBackGraph << "\"];\n";
      if (root_ != nil_) {
        file_graph_ << "All:<f0> -> node" << root_ << "[color = \"" << ColorEdgeHead << "\"];\n";
      }
    }
    void WriteNil() {
      file_graph_ << "\tnode" << nil_ << "[shape = Mrecord, style = filled, fillcolor = \"" << ColorBlack << "\", color = \""
                  << ColorEdgeGraph << "\", label = \"{ PARENT: " << nil_->getParent() << " | PTR: " << nil_ << " | NIL "
                  << " | SIZE: " << nil_->getSize() << " | { <f0> LEFT: " << nil_->getLeft()
                  << " | <f1> RIGHT: " << nil_->getRight() << " }}\"];\n";
    }
};
}; // namespace Tree

#define printTree() printLogTree(__FILE__, __PRETTY_FUNCTION__, __LINE__)

#endif