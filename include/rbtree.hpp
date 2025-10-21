#ifndef INCLUDE_RBTREE_HPP
#define INCLUDE_RBTREE_HPP

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Tree {
/// Requires bool comparator_(KeyT a, KeyT b)
/// If a < b gives true
/// else false
template <typename KeyT, typename Comp> class RBTree {
    enum class Color { Red = 0, Black = 1 };
    class Node {
        KeyT key_;
        Node *parent_ = nullptr;
        Node *left_ = nullptr;
        Node *right_ = nullptr;
        Color color_ = Color::Black;

      public:
        Node() = default;
        Node(const Node &) = default;
        Node &operator=(const Node &) = default;

        Node(const KeyT &key) : key_(key) {}

        const KeyT &getKey() const { return key_; }

        void setParent(Node *parent) { parent_ = parent; }
        void setLeft(Node *left) { left_ = left; }
        void setRight(Node *right) { right_ = right; }
        void setColor(Color color) { color_ = color; }

        Node *getParent() const { return parent_; }
        Node *getLeft() const { return left_; }
        Node *getRight() const { return right_; }
        Color getColor() const { return color_; }

        bool isRed() { return color_ == Color::Red; }
        bool isBlack() { return color_ == Color::Black; }
    };

    Node nil_node_;           // Nill object that points to itself
    Node *nil_ = &nil_node_;  // pointer of nill
    Node *root_ = &nil_node_; // root
    Comp comparator_;         // comparator

  private:
    /// Method to search key in tree
    const Node *search(const KeyT &key) const { return search(root_, key); }
    /// Method to search key in tree's node using loop(not recursion)
    const Node *search(const Node *node, const KeyT &key) const {
      while ((node != nil_) && (key != node->getKey())) {
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
      std::cout << node->getKey() << std::endl;
      inorderTreeWalk(node->getRight());
    }

    /// Method finds minimum node in tree
    const Node *minimum() const { return minimum(root_); }
    /// Method finds minimum node from tree's node
    const Node *minimum(const Node *node) const {
      while (node->getLeft() != nil_)
        node = node->getLeft();
      return node;
    }
    /// Method finds maximum node in tree
    const Node *maximum() const { return maximum(root_); }
    /// Method finds maximum node from tree's node
    const Node *maximum(const Node *node) const {
      while (node->getRight() != nil_)
        node = node->getRight();
      return node;
    }

    /// Method of find "next" node from given
    /// If maximum - return NIL
    const Node *successor(const Node *node) const {
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
    const Node *predecessor(const Node *node) const {
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
    void rotateLeft(Node *x) {
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
    }

    /// Method rotates node for right
    /// Requires left leaf != NIL
    void rotateRight(Node *x) {
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
    }

    void insertFixup(Node *z) {
      while (z->getParent()->isRed()) {
        if (z->getParent() == z->getParent()->getParent()->getLeft()) {
          Node *y = z->getParent()->getParent()->getRight();
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
          Node *y = z->getParent()->getParent()->getLeft();
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
    /// Methods for use tree
  public:
    RBTree(Comp comparator, bool enable_log = false) : comparator_(comparator), enable_log_(enable_log) {
      nil_node_.setColor(Color::Black);
      nil_node_.setLeft(&nil_node_);
      nil_node_.setRight(&nil_node_);
      nil_node_.setParent(&nil_node_);

      printLogStart();
    }
    ~RBTree() { printLogFinish(); }

    void insert(const KeyT &key) {
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
    }
    /// Method to walk tree recursively
    void inorderTreeWalk() { inorderTreeWalk(root_); }

    /// Grpahviz log section
  private:
    static constexpr const char *FolderLogPath = "/tmp/rbtree_log/";
    static constexpr const char *FileLogPath = "/tmp/rbtree_log/log.html";
    static constexpr const char *FileGraphPath = "/tmp/rbtree_log/graph.dot";
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
    std::ofstream file_log_;
    std::ofstream file_graph_;

  public:
    void printLogTree(const char *file, const char *function, const size_t line) {
      if (!enable_log_)
        return;

      GenerateGraph();

      // Run: dot <path to dot graph> -Tsvg

      // Args for dot
      std::vector<std::string> args{};
      args.emplace_back(FileGraphPath);
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
    void printLogStart(void) {
      // If not logs
      if (!enable_log_)
        return;

      const boost::filesystem::path log_dir = boost::filesystem::path(FolderLogPath);
      const bool need_init = !boost::filesystem::exists(log_dir) || !boost::filesystem::is_directory(log_dir);
      if (need_init) {
        boost::system::error_code ec;
        boost::filesystem::create_directories(log_dir, ec);
        if (ec)
          throw std::runtime_error("creating path for logs failed: " + ec.message());
      }
      file_log_.open(FileLogPath, std::ios::out | std::ios::trunc);

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
      file_graph_.open(FileGraphPath, std::ios::out | std::ios::trunc);

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
                    << " | PTR: " << CurrentNode << " | DATA: " << CurrentNode->getKey()
                    << " | { <f0> LEFT: " << CurrentNode->getLeft() << " | <f1> RIGHT: " << CurrentNode->getRight()
                    << " }}\"];\n";
      }
      if (CurrentNode->isRed()) {
        file_graph_ << "\tnode" << CurrentNode << "[shape = Mrecord, style = filled, fillcolor = \"" << ColorRed
                    << "\", color = \"" << ColorEdgeGraph << "\", label = \"{ PARENT: " << CurrentNode->getParent()
                    << " | PTR: " << CurrentNode << " | DATA: " << CurrentNode->getKey()
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
                  << " | { <f0> LEFT: " << nil_->getLeft() << " | <f1> RIGHT: " << nil_->getRight() << " }}\"];\n";
    }
};
}; // namespace Tree

#define printTree() printLogTree(__FILE__, __PRETTY_FUNCTION__, __LINE__)

#endif