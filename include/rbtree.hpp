#ifndef INCLUDE_RBTREE_HPP
#define INCLUDE_RBTREE_HPP

#include <iostream>
#include <stack>
#include <utility>

#ifdef VIS_LOGS
#include <boost/process.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#endif

namespace Tree {
/// Requires bool comparator_(KeyT a, KeyT b)
/// If a < b gives true
/// else == (a <= b) gives false
template <typename KeyT, typename Comp> class RBTree {
    class Node {
        KeyT key_;
        Node *parent_ = nullptr;
        Node *left_ = nullptr;
        Node *right_ = nullptr;
        bool color_black_ = true; // bool "is node black?"
        size_t size_ = 1;

      public:
        Node() = default;
        Node(const Node &) = default;
        Node &operator=(const Node &) = default;

        Node(const KeyT &key) : key_(key) {}

        inline const KeyT &getKey() const noexcept { return key_; }

        friend class RBTree;
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

#ifdef VIS_LOGS
      // Close previous logs
      closeLogger();
      // Copy logger
      copyLogger(rhs);
#endif

      // Src to dst values coping
      nil_node_.color_black_ = true;
      nil_node_.left_ = &nil_node_;
      nil_node_.right_ = &nil_node_;
      nil_node_.parent_ = &nil_node_;
      nil_node_.size_ = 0;
      nil_ = &nil_node_;
      root_ = &nil_node_;
      comparator_ = rhs.comparator_;

      if (rhs.root_ == rhs.nil_)
        return;

      Node *new_root = new Node(rhs.root_->key_);
      new_root->color_black_ = true;
      new_root->left_ = nil_;
      new_root->right_ = nil_;
      new_root->parent_ = nil_;
      new_root->size_ = rhs.root_->size_;
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
        if (src->left_ != rhs.nil_) {
          Node *child = new Node(src->left_->key_);
          dst->left_ = child;
          child->color_black_ = src->left_->color_black_;
          child->left_ = nil_;
          child->right_ = nil_;
          child->size_ = src->left_->size_;
          child->parent_ = dst;
          nodes_stack.push({src->left_, child});
        } else {
          dst->left_ = nil_;
        }
        // Right child
        if (src->right_ != rhs.nil_) {
          Node *child = new Node(src->right_->key_);
          dst->right_ = child;
          child->color_black_ = src->right_->color_black_;
          child->left_ = nil_;
          child->right_ = nil_;
          child->size_ = src->right_->size_;
          child->parent_ = dst;
          nodes_stack.push({src->right_, child});
        } else {
          dst->right_ = nil_;
        }
      }
    }

    /// Update sizes for all branch
    inline void updateSizeUp(Node *node) noexcept {
      while (node != nil_) {
        node->size_ = 1 + node->left_->size_ + node->right_->size_;
        node = node->parent_;
      }
    }

    /// Method returns an ptr to the first element that compares not less than x.
    inline Node *lowerBound(const KeyT &key) const noexcept { return lowerBound(root_, key); }
    inline Node *lowerBound(Node *node, const KeyT &key) const noexcept {
      Node *save_node = nil_;
      while (node != nil_) {
        if (!comparator_(node->key_, key)) {
          save_node = node;
          node = node->left_;
        } else {
          node = node->right_;
        }
      }
      return save_node;
    }

    /// Method returns an ptr to the first element that compares greater than x.
    inline Node *upperBound(const KeyT &key) const noexcept { return upperBound(root_, key); }
    inline Node *upperBound(Node *node, const KeyT &key) const noexcept {
      Node *save_node = nil_;
      while (node != nil_) {
        if (comparator_(key, node->key_)) {
          save_node = node;
          node = node->left_;
        } else {
          node = node->right_;
        }
      }
      return save_node;
    }

    inline size_t rankLowerBound(const KeyT &key) const noexcept {
      size_t rank = 0;
      Node *node = root_;

      while (node != nil_) {
        if (comparator_(node->key_, key)) {
          rank += 1 + node->left_->size_;
          node = node->right_;
        } else {
          node = node->left_;
        }
      }

      return rank;
    }

    inline size_t rankUpperBound(const KeyT &key) const noexcept {
      size_t rank = 0;
      Node *node = root_;

      while (node != nil_) {
        if (!comparator_(key, node->key_)) {
          rank += 1 + node->left_->size_;
          node = node->right_;
        } else {
          node = node->left_;
        }
      }

      return rank;
    }

    /// Method of free tree's nodes
    void freeTree() noexcept {
      if (root_ == nil_)
        return;

      std::stack<Node *> node_stack;
      node_stack.push(root_);
      while (!node_stack.empty()) {
        Node *delete_node = node_stack.top();
        node_stack.pop();

        if (delete_node->left_ != nil_)
          node_stack.push(delete_node->left_);
        if (delete_node->right_ != nil_)
          node_stack.push(delete_node->right_);

        delete delete_node;
      }
    }

    /// Method to search key in tree
    /// Return non-const node ptr
    inline Node *search(const KeyT &key) const noexcept { return search(root_, key); }

    inline Node *search(Node *node, const KeyT &key) const noexcept {
      while (node != nil_) {
        if (comparator_(key, node->key_)) {
          node = node->left_;
        } else if (comparator_(node->key_, key)) {
          node = node->right_;
        } else {
          return node;
        }
      }
      return node;
    }

    /// Method to walk tree's node recursively
    void inorderTreeWalk(const Node *node) const {
      if (node == nil_)
        return;
      inorderTreeWalk(node->left_);
      std::cout << node->key_ << '\n';
      inorderTreeWalk(node->right_);
    }

    /// Method finds minimum node in tree
    inline Node *minimum() const noexcept { return minimum(root_); }
    /// Method finds minimum node from tree's node
    inline Node *minimum(Node *node) const noexcept {
      while (node->left_ != nil_)
        node = node->left_;
      return node;
    }
    /// Method finds maximum node in tree
    inline const Node *maximum() const noexcept { return maximum(root_); }
    /// Method finds maximum node from tree's node
    inline const Node *maximum(const Node *node) const noexcept {
      while (node->right_ != nil_)
        node = node->right_;
      return node;
    }

    /// Method of find "next" node from given
    /// If maximum - return NIL
    inline const Node *successor(const Node *node) const noexcept {
      if (node->right_ != nil_)
        return minimum(node->right_);

      Node *save_node = node->parent_;
      while ((save_node != nil_) && (node == save_node->right_)) {
        node = save_node;
        save_node = save_node->parent_;
      }
      return save_node;
    }

    /// Method of find "previous" node from given
    /// If minimum - return NIL
    inline const Node *predecessor(const Node *node) const noexcept {
      if (node->left_ != nil_)
        return maximum(node->left_);

      Node *save_node = node->parent_;
      while ((save_node != nil_) && (node == save_node->left_)) {
        node = save_node;
        save_node = save_node->parent_;
      }
      return save_node;
    }

    /// Method rotates node for left
    /// Requires right leaf != NIL
    inline void rotateLeft(Node *x) noexcept {
      Node *y = x->right_;  // set y
      x->right_ = y->left_; // Make y left subtree in right for x

      if (y->left_ != nil_) {
        y->left_->parent_ = x;
      }

      y->parent_ = x->parent_; // Move parent of x to y

      if (x->parent_ == nil_) {
        root_ = y;
      } else if (x == x->parent_->left_) {
        x->parent_->left_ = y;
      } else {
        x->parent_->right_ = y;
      }

      y->left_ = x; // Make x - left for y
      x->parent_ = y;

      // Update sizes
      x->size_ = 1 + x->left_->size_ + x->right_->size_;
      y->size_ = 1 + y->left_->size_ + y->right_->size_;
    }

    /// Method rotates node for right
    /// Requires left leaf != NIL
    inline void rotateRight(Node *x) noexcept {
      Node *y = x->left_;   // set y
      x->left_ = y->right_; // Make y right subtree in left for x

      if (y->right_ != nil_) {
        y->right_->parent_ = x;
      }

      y->parent_ = x->parent_; // Move parent of x to y

      if (x->parent_ == nil_) {
        root_ = y;
      } else if (x == x->parent_->left_) {
        x->parent_->left_ = y;
      } else {
        x->parent_->right_ = y;
      }

      y->right_ = x; // Make x - right for y
      x->parent_ = y;

      // Update sizes
      x->size_ = 1 + x->left_->size_ + x->right_->size_;
      y->size_ = 1 + y->left_->size_ + y->right_->size_;
    }

    inline void insertFixup(Node *z) noexcept {
      Node *y = nil_;
      while (!z->parent_->color_black_) {
        if (z->parent_ == z->parent_->parent_->left_) {
          y = z->parent_->parent_->right_;
          if (!y->color_black_) {
            z->parent_->color_black_ = true;
            y->color_black_ = true;
            z->parent_->parent_->color_black_ = false;
            z = z->parent_->parent_;
          } else {
            if (z == z->parent_->right_) {
              z = z->parent_;
              rotateLeft(z);
            }
            z->parent_->color_black_ = true;
            z->parent_->parent_->color_black_ = false;
            rotateRight(z->parent_->parent_);
          }
        } else {
          y = z->parent_->parent_->left_;
          if (!y->color_black_) {
            z->parent_->color_black_ = true;
            y->color_black_ = true;
            z->parent_->parent_->color_black_ = false;
            z = z->parent_->parent_;
          } else {
            if (z == z->parent_->left_) {
              z = z->parent_;
              rotateRight(z);
            }
            z->parent_->color_black_ = true;
            z->parent_->parent_->color_black_ = false;
            rotateLeft(z->parent_->parent_);
          }
        }
      }
      root_->color_black_ = true;
    }

    inline void transplant(Node *u, Node *v) noexcept {
      if (u->parent_ == nil_) {
        root_ = v;
      } else if (u == u->parent_->left_) {
        u->parent_->left_ = v;
      } else {
        u->parent_->right_ = v;
      }
      v->parent_ = u->parent_;
    }

    inline void eraseFixup(Node *x) noexcept {
      Node *w = nil_;
      while ((x != root_) && x->color_black_) {
        if (x == x->parent_->left_) {
          // w is x's sibling
          w = x->parent_->right_;

          if (!w->color_black_) {
            w->color_black_ = true;
            x->parent_->color_black_ = false;
            rotateLeft(x->parent_);
            w = x->parent_->right_;
          }
          if ((w->left_->color_black_) && (w->right_->color_black_)) {
            w->color_black_ = false;
            x = x->parent_;
          } else {
            if (w->right_->color_black_) {
              w->left_->color_black_ = true;
              w->color_black_ = false;
              rotateRight(w);
              w = x->parent_->right_;
            }
            w->color_black_ = x->parent_->color_black_;
            x->parent_->color_black_ = true;
            w->right_->color_black_ = true;
            rotateLeft(x->parent_);
            x = root_;
          }
        } else {
          // w is x's sibling
          w = x->parent_->left_;

          if (!w->color_black_) {
            w->color_black_ = true;
            x->parent_->color_black_ = false;
            rotateRight(x->parent_);
            w = x->parent_->left_;
          }
          if (w->right_->color_black_ && w->left_->color_black_) {
            w->color_black_ = false;
            x = x->parent_;
          } else {
            if (w->left_->color_black_) {
              w->right_->color_black_ = true;
              w->color_black_ = false;
              rotateLeft(w);
              w = x->parent_->left_;
            }
            w->color_black_ = x->parent_->color_black_;
            x->parent_->color_black_ = true;
            w->left_->color_black_ = true;
            rotateRight(x->parent_);
            x = root_;
          }
        }
      }
      x->color_black_ = true;
    }

    /// Method for deleting
    inline void erase(Node *z) noexcept {
      Node *y = z;
      Node *x = nil_;
      bool y_orig_color = y->color_black_;
      if (z->left_ == nil_) {
        x = z->right_;
        transplant(z, z->right_); // replace z by its right child
      } else if (z->right_ == nil_) {
        x = z->left_;
        transplant(z, z->left_); // replace z by its left child
      } else {
        y = minimum(z->right_); // y is z’s successor
        y_orig_color = y->color_black_;
        x = y->right_;
        if (y != z->right_) {
          transplant(y, y->right_);
          y->right_ = z->right_;  // z’s right child becomes
          y->right_->parent_ = y; // y’s right child
        } else {
          x->parent_ = y; // in case x is NIL
        }
        transplant(z, y);      // replace z by its successor y
        y->left_ = z->left_;   // and give z’s left child to y,
        y->left_->parent_ = y; // which had no left child
        y->color_black_ = z->color_black_;
      }
      // if any red-black violations occured, correct them
      if (y_orig_color) {
        eraseFixup(x);
      }

      // Update sizes
      Node *n = x;
      if (n == nil_)
        n = x->parent_; // if nil - get to its parent

      updateSizeUp(n);

      // Revive nil_'s parent to nil_(changes was required by algorithm)
      // And keep root black
      nil_->parent_ = nil_;
      root_->color_black_ = true;
      delete z;
    }

    std::pair<bool, int> checkNode(const Node *n) const noexcept {
      if (n == nil_)
        return {true, 1}; // NIL is black, contributes 1

      auto [l_ok, l_bh] = checkNode(n->left_);
      auto [r_ok, r_bh] = checkNode(n->right_);

      // Strict BST property with comparator: left < n < right
      bool bst_ok = (n->left_ == nil_ || comparator_(n->left_->key_, n->key_)) &&
                    (n->right_ == nil_ || comparator_(n->key_, n->right_->key_));

      // Red node must have black children
      bool red_ok = !(!n->color_black_ && (!n->left_->color_black_ || !n->right_->color_black_));

      // Same black height on both sides
      bool bh_ok = (l_bh == r_bh);

      int bh = l_bh + n->color_black_;
      return {l_ok && r_ok && bst_ok && red_ok && bh_ok, bh};
    }

    /// Method for verifing RB invariants
    bool checkInvariants() const noexcept {
      if (root_ == nil_)
        return true; // empty tree is valid
      if (!root_->color_black_)
        return false; // root must be black
      return checkNode(root_).first;
    }

    /// Methods for use tree
  public:
    RBTree(Comp comparator
#ifdef VIS_LOGS
           ,
           bool enable_log = false
#endif
           )
        : comparator_(comparator)
#ifdef VIS_LOGS
          ,
          enable_log_(enable_log)
#endif
    {
      nil_node_.color_black_ = true;
      nil_node_.left_ = &nil_node_;
      nil_node_.right_ = &nil_node_;
      nil_node_.parent_ = &nil_node_;
      nil_node_.size_ = 0;
#ifdef VIS_LOGS
      printLogStart();
#endif
    }

    RBTree(const RBTree &rhs) { copyTree(rhs); }

    RBTree &operator=(const RBTree &rhs) {
      copyTree(rhs);
      return *this;
    }

    ~RBTree() {
      freeTree();

#ifdef VIS_LOGS
      printLogFinish();
#endif
    }

    inline bool contains(const KeyT &key) const noexcept { return search(root_, key) != nil_; }

    inline void insert(const KeyT &key) noexcept {
      Node *y = nil_;
      Node *x = root_;

      while (x != nil_) {
        y = x;
        ++x->size_;

        if (comparator_(key, x->key_)) {
          x = x->left_;
        } else if (comparator_(x->key_, key)) {
          x = x->right_;
        } else {
          --y->size_;

          Node *p = y->parent_;
          while (p != nil_) {
            --p->size_;
            p = p->parent_;
          }
          return;
        }
      }

      Node *z = new Node(key);
      z->parent_ = y;
      z->left_ = nil_;
      z->right_ = nil_;
      z->color_black_ = false;
      z->size_ = 1;

      if (y == nil_) {
        root_ = z;
      } else if (comparator_(key, y->key_)) {
        y->left_ = z;
      } else {
        y->right_ = z;
      }

      insertFixup(z);
    }

    inline size_t erase(const KeyT &key) noexcept {
      Node *find_node = search(key);
      if (find_node == nil_)
        return 0;
      erase(find_node);

      // if (!checkInvariants())
      //   throw std::runtime_error("Bad Red Black tree, no invariants");

      return 1;
    }

    /// Method count elements in range [begin, end]
    inline size_t rangeQuery(const KeyT &begin, const KeyT &end) const noexcept {
      if (comparator_(end, begin))
        return 0;
      return rankUpperBound(end) - rankLowerBound(begin);
    }

    /// Method to walk tree recursively
    void inorderTreeWalk() const noexcept { inorderTreeWalk(root_); }

    /// Grpahviz log section

#ifdef VIS_LOGS
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
    std::filesystem::path log_dir;
    std::filesystem::path graph_path;
    std::filesystem::path log_path;

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
      int exit_code = 0;
      try {
        exit_code = boost::process::system("dot", boost::process::args(args), boost::process::std_out > dot_out);
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

    void closeLogger() { printLogFinish(); }

    void printLogStart(void) {
      // If disable logging
      if (!enable_log_)
        return;

      std::ostringstream oss;
      oss << static_cast<const void *>(nil_);
      log_dir = std::filesystem::path(FolderLogPath) / std::filesystem::path(oss.str());
      bool need_init = !std::filesystem::exists(log_dir) || !std::filesystem::is_directory(log_dir);
      if (need_init) {
        std::error_code ec;
        std::filesystem::create_directories(log_dir, ec);
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
      if (CurrentNode->color_black_) {
        file_graph_ << "\tnode" << CurrentNode << "[shape = Mrecord, style = filled, fillcolor = \"" << ColorBlack
                    << "\", color = \"" << ColorEdgeGraph << "\", label = \"{ PARENT: " << CurrentNode->parent_
                    << " | PTR: " << CurrentNode << " | DATA: " << CurrentNode->key_ << " | SIZE: " << CurrentNode->getSize()
                    << " | { <f0> LEFT: " << CurrentNode->left_ << " | <f1> RIGHT: " << CurrentNode->right_ << " }}\"];\n";
      }
      if (!CurrentNode->color_black_) {
        file_graph_ << "\tnode" << CurrentNode << "[shape = Mrecord, style = filled, fillcolor = \"" << ColorRed
                    << "\", color = \"" << ColorEdgeGraph << "\", label = \"{ PARENT: " << CurrentNode->parent_
                    << " | PTR: " << CurrentNode << " | DATA: " << CurrentNode->key_ << " | SIZE: " << CurrentNode->getSize()
                    << " | { <f0> LEFT: " << CurrentNode->left_ << " | <f1> RIGHT: " << CurrentNode->right_ << " }}\"];\n";
      }
      if (CurrentNode->left_ != nil_) {
        WriteNode(CurrentNode->left_);
        file_graph_ << "\tnode" << CurrentNode << ": <f0> -> node" << CurrentNode->left_ << "[color = \"" << ColorEdgeGraph
                    << "\"]\n";
      }
      if (CurrentNode->right_ != nil_) {
        WriteNode(CurrentNode->right_);
        file_graph_ << "\tnode" << CurrentNode << ": <f1> -> node" << CurrentNode->right_ << "[color = \"" << ColorEdgeGraph
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
                  << ColorEdgeGraph << "\", label = \"{ PARENT: " << nil_->parent_ << " | PTR: " << nil_ << " | NIL "
                  << " | SIZE: " << nil_->getSize() << " | { <f0> LEFT: " << nil_->left_ << " | <f1> RIGHT: " << nil_->right_
                  << " }}\"];\n";
    }
#endif
};
}; // namespace Tree

#ifdef VIS_LOGS
#define printTree() printLogTree(__FILE__, __PRETTY_FUNCTION__, __LINE__)
#endif

#endif