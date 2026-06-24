#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <vector>

#include <string>

template<class T> using vec = std::vector<T>;

// bad order, fix later
struct Node {
  bool isleaf;
  std::string name; // text for leaves and tag name for nodes
  vec<Node*> kids;
  vec<std::pair<std::string, std::string>> attrib;

  Node(): isleaf(true) {}
  Node( const std::string& text ): isleaf(true), name(text) {}
  ~Node() {
    for( const Node* kid : kids )
      delete kid;
  }
};

void dump( const Node *node, int lvl = 2 );

Node* parse_html( const std::string& data );







