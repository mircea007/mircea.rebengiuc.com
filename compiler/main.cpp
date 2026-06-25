// really funny template engine (and html parser)
// don't judge, my lazyness is beyond understanding

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <vector>

#include <map>
#include <string>

#include "compiler.hpp"

template<class T> using vec = std::vector<T>;

// who knew?
std::string read_file( const char *fname ) {
  FILE *fin = fopen( fname, "rb" );
  if( !fin ) return "";

  fseek( fin, 0, SEEK_END );
  size_t lenght = ftell( fin );
  fseek( fin, 0, SEEK_SET );

  char* ret = (char*)malloc( lenght );
  assert( ret );

  fread( ret, 1, lenght, fin );
  fclose( fin );

  std::string _ret(ret, ret + lenght);
  free( ret );
  return _ret;
}

void string_replace( std::string& text, const std::string& pat, const std::string& sub ) {
  if( text.size() < pat.size() ) return;

  int idx = 0;
  while( idx + (int)pat.size() <= (int)text.size() ){
    if( !std::equal( pat.begin(), pat.end(), text.begin() + idx ) ){
      idx++;
      continue;
    }

    text.erase( text.begin() + idx, text.begin() + idx + (int)pat.size() );
    text.insert( text.begin() + idx, sub.begin(), sub.end() );
    idx += (int)sub.size();
  }
}

int main( int argc, char *argv[] ) {
  assert( argc == 1 + 2 );

  Node* input_tree = parse_html( read_file( argv[1] ) );
  Node* template_tree = parse_html( read_file( argv[2] ) );

  assert( template_tree->name == "all" );
  std::map<std::string, Node*> name2node;
  for( Node* template_node : template_tree->kids ){
    assert( template_node->name == "template" );
    assert( (int)template_node->attrib.size() == 1 && template_node->attrib[0].first == "id" );
    std::string name = template_node->attrib[0].second;
    name2node[name] = template_node;
  }

  auto replace = [&]( auto&& self, Node* root ) -> std::string {
    if( !root ) return "";
    if( root->isleaf ) return root->name;

    Node* temp_node = nullptr; {
      int idx = 0;
      while( idx < (int)root->attrib.size() && root->attrib[idx].first != "temp-import" )
        idx++;
      if( idx < (int)root->attrib.size() && name2node.count( root->attrib[idx].second ) )
        temp_node = name2node[root->attrib[idx].second];
    }

    std::string content;
    for( Node* kid : root->kids )
      content += self( self, kid );
    
    if( !temp_node ){
      std::string ret(1, '<');
      ret += root->name;

      for( const auto& [key, val]: root->attrib ){
        ret += " ";
        ret += key;
        ret += "=\"";
        ret += val;
        ret += "\"";
      }

      ret += ">";
      ret += content;
      ret += "</";
      ret += root->name;
      ret += ">";
      return ret;
    }

    std::string sub;
    for( Node* kid : temp_node->kids )
      sub += make_string( kid );

    string_replace( sub, "__content__", content );
    std::string pref = "temp-";
    for( const auto& [key, val]: root->attrib ){
      if( !std::equal( pref.begin(), pref.end(), key.begin() ) ) continue;
      std::string pat = "__" + std::string(key.begin() + pref.size(), key.end()) + "__";
      string_replace( sub, pat, val );
    }

    return sub;
  };

  std::string output = replace( replace, input_tree );
  std::string pref = "<all>", suff = "</all>";
  assert( std::equal( pref.begin(), pref.end(), output.begin() ) );
  assert( std::equal( suff.rbegin(), suff.rend(), output.rbegin() ) );
  output.erase( output.begin(), output.begin() + pref.size() );
  output.erase( output.end() - suff.size(), output.end() );

  printf( "%s\n", output.c_str() );

  delete input_tree;
  delete template_tree;
  return 0;
}






