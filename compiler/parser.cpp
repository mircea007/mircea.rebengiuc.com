// really funny html parser
// don't judge heavy use of std::string
// my lazyness is beyond understanding

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <vector>

#include <string>

#include "compiler.hpp"

template<class T> using vec = std::vector<T>;

void dump( Node *node, int lvl ) {
  if( !node ) return;
  for( int t = lvl; t--; ) fputc( ' ', stdout );

  printf( "(%d \"%s\")", int(node->isleaf), node->name.c_str() );

  for( const auto &[key, val]: node->attrib )
    printf( " \"%s\" = \"%s\"", key.c_str(), val.c_str() );
  printf( "\n" );

  for( Node* kid : node->kids )
    dump( kid, lvl + 4 );
}

std::string make_string( Node* root ) {
  if( !root ) return "";
  if( root->isleaf ) return root->name;

  std::string ret(1, '<');
  ret += root->name;

  for( const auto &[key, val]: root->attrib ){
    ret += " ";
    ret += key;
    ret += "=\"";
    ret += val;
    ret += "\"";
  }

  ret += ">";
  for( Node* kid : root->kids )
    ret += make_string( kid );

  ret += "</";
  ret += root->name;
  ret += ">";
  return ret;
}

Node* copy_node( Node* root ) {
  if( !root ) return nullptr;
  Node* ret = new Node(*root);
  for( auto& kid : ret->kids )
    kid = copy_node( kid );
  return ret;
}

struct Token {
  enum class Type {
    TAG_BEGIN, TAG_END, TAG_POP, TAG_POP_SELF,
    TEXT, ATTRIB, EQUAL, STRING_LITERAL
  } type;
  std::string value;

  std::string print() const {
    if( type == Type::TEXT ) return value;
    if( type == Type::ATTRIB ) return value;
    if( type == Type::STRING_LITERAL ) return "\"" + value + "\"";
    if( type == Type::TAG_BEGIN ) return "<";
    if( type == Type::TAG_END ) return ">";
    if( type == Type::TAG_POP ) return "</";
    if( type == Type::TAG_POP_SELF ) return "/>";
    if( type == Type::EQUAL ) return "=";
    assert( false );
  }
};

template<class IT>
vec<Token> tokenize_raw( IT begin, IT end ) {
  vec<Token> ret;
  IT it = begin;
  auto getc = [&]() -> char { assert( it != end ); return *(it++); };

  bool inside_tag = false;
  bool inside_string_literal = false;
  bool begin_new_token = true;
  while( it != end ){
    char ch = getc();

    if( !inside_tag ){
      assert( !inside_string_literal );

      if( ch == '<' ){
        ret.push_back( Token{ Token::Type::TAG_BEGIN, "" } );
        begin_new_token = true;
        inside_tag = true;
        continue;
      }

      if( !begin_new_token ){
        assert( !ret.empty() && ret.back().type == Token::Type::TEXT );
        ret.back().value.push_back( ch );
        continue;
      }

      if( !isspace( ch ) ){
        ret.push_back( Token{ Token::Type::TEXT, std::string(1, ch) } );
        begin_new_token = false;
        continue;
      }

      continue;
    }

    if( inside_string_literal ){
      assert( !ret.empty() && ret.back().type == Token::Type::STRING_LITERAL );

      if( ch == '"' ){
        inside_string_literal = false;
        begin_new_token = true;
        continue;
      }

      ret.back().value.push_back( ch );
      if( ch == '\\' )
        ret.back().value.push_back( getc() );
      continue;
    }

    // inside tag and not string literal

    if( ch == '"' ){
      inside_string_literal = true;
      begin_new_token = false;
      ret.push_back( Token{ Token::Type::STRING_LITERAL, "" } );
      continue;
    }

    if( isspace( ch ) ){
      begin_new_token = true;
      continue;
    }

    if( ch == '>' ){
      ret.push_back( Token{ Token::Type::TAG_END, "" } );
      inside_tag = false;
      begin_new_token = true;
      continue;
    }

    if( begin_new_token ){
      ret.push_back( Token{ Token::Type::ATTRIB, "" } );
      begin_new_token = false;
    }

    assert( !ret.empty() && ret.back().type == Token::Type::ATTRIB );
    ret.back().value.push_back( ch );
    continue;
  }

  return ret;
}

// make attrib A= -> attrib A, equal;
// make attrib =  -> equal;
// make tag_begin, attrib /* -> tag_pop;
// make attrib /, tag_end -> tag_pop_self;
vec<Token> post_process( const vec<Token>& tokens ) {
  vec<Token> ret;

  for( const Token& T : tokens ){
    if( T.type == Token::Type::ATTRIB ){
      assert( !T.value.empty() );

      if( T.value == "=" ){
        ret.push_back( Token{ Token::Type::EQUAL, "" } );
        continue;
      }

      if( T.value.back() == '=' ){
        ret.push_back( Token{ Token::Type::ATTRIB, std::string(T.value.begin(), T.value.end() - 1) } );
        ret.push_back( Token{ Token::Type::EQUAL, "" } );
        continue;
      }

      if( T.value.front() == '/' && (int)T.value.size() >= 2 && !ret.empty() && ret.back().type == Token::Type::TAG_BEGIN ){
        ret.pop_back();
        ret.push_back( Token{ Token::Type::TAG_POP, "" } );
        ret.push_back( Token{ Token::Type::ATTRIB, std::string(T.value.begin() + 1, T.value.end()) } );
        continue;
      }

      ret.push_back( T );
      continue;
    }

    if( T.type == Token::Type::TAG_END ){
      if( ret.empty() || ret.back().type != Token::Type::ATTRIB || ret.back().value != "/" ){
        ret.push_back( T );
        continue;
      }

      ret.pop_back();
      ret.push_back( Token{ Token::Type::TAG_POP_SELF, "" } );
      continue;
    }

    if( T.type == Token::Type::TEXT ){
      ret.push_back( T );
      Token& _T = ret.back();
      while( !_T.value.empty() && isspace( _T.value.back() ) )
        _T.value.pop_back();
      continue;
    }

    ret.push_back( T );
  }
  
  return ret;
}

std::string pre_process( const std::string& data ) {
  vec<bool> in_comment(data.size(), false); {
    bool state = false;
    for( int i = 0; i < (int)data.size(); i++ ){
      if( i - 3 >= 0 && std::string( data.begin() + i - 3, data.begin() + i ) == "-->" )
        state = false;
      if( i + 4 <= (int)data.size() && std::string( data.begin() + i, data.begin() + i + 4 ) == "<!--" )
        state = true;
      
      in_comment[i] = state;
    }
  }

  std::string ret; {
    bool prev = false;
    for( int i = 0; i < (int)data.size(); i++ ){
      if( in_comment[i] ){
        if( !prev )
          ret.push_back( ' ' );
        prev = true;
      }else{
        ret.push_back( data[i] );
        prev = false;
      }
    }
  }

  return ret;
}

Node* make_tree( const vec<Token>& tokens ) {
  int idx = 0;

  auto parse_node = [&]( auto&& self ) -> Node* {
    assert( idx < (int)tokens.size() && tokens[idx].type == Token::Type::TAG_BEGIN ); idx++;
    assert( idx < (int)tokens.size() && tokens[idx].type == Token::Type::ATTRIB ); idx++;

    Node *root = new Node();
    root->isleaf = false;
    root->name = tokens[idx - 1].value;

    while( idx < (int)tokens.size() && !(tokens[idx].type == Token::Type::TAG_END || tokens[idx].type == Token::Type::TAG_POP_SELF) ){
      assert( tokens[idx].type == Token::Type::ATTRIB );
      
      if( idx + 1 < (int)tokens.size() && tokens[idx + 1].type != Token::Type::EQUAL ){
        root->attrib.emplace_back( tokens[idx].value, "" );
        idx++;
        continue;
      }

      assert( idx + 2 < (int)tokens.size() && tokens[idx + 2].type == Token::Type::STRING_LITERAL );
      root->attrib.emplace_back( tokens[idx].value, tokens[idx + 2].value );
      idx += 3;
      continue;
    }

    assert( idx < (int)tokens.size() );
    idx++;

    if( tokens[idx - 1].type == Token::Type::TAG_POP_SELF )
      return root;

    while( idx < (int)tokens.size() && tokens[idx].type != Token::Type::TAG_POP ){
      if( tokens[idx].type == Token::Type::TEXT ){
        root->kids.push_back( new Node(tokens[idx].value) );
        idx++;
        continue;
      }

      if( tokens[idx].type == Token::Type::TAG_BEGIN ){
        root->kids.push_back( self( self ) );
        continue;
      }

      assert( false );
    }

    assert( idx < (int)tokens.size() ); idx++;
    assert( idx < (int)tokens.size() && tokens[idx].type == Token::Type::ATTRIB );
    if( tokens[idx].value != root->name ){
      fprintf( stderr, "trying to close \"%s\" with \"%s\"\n", tokens[idx].value.c_str(), root->name.c_str() );
      assert( false );
    }

    idx++;
    assert( idx < (int)tokens.size() && tokens[idx].type == Token::Type::TAG_END ); idx++;
    return root;
  };
  
  Node* root = parse_node( parse_node );
  // assert( idx == (int)tokens.size() );
  return root;
}

Node* parse_html( const std::string& data ) {
  std::string sanitized = pre_process( data );
  vec<Token> tokens_raw = tokenize_raw( sanitized.begin(), sanitized.end() );
  vec<Token> tokens = post_process( tokens_raw );

  vec<Token> pref = {
    Token{ Token::Type::TAG_BEGIN, "" },
    Token{ Token::Type::ATTRIB, "all" },
    Token{ Token::Type::TAG_END, "" }
  }, suff = {
    Token{ Token::Type::TAG_POP, "" },
    Token{ Token::Type::ATTRIB, "all" },
    Token{ Token::Type::TAG_END, "" }
  };

  tokens.insert( tokens.begin(), pref.begin(), pref.end() );
  tokens.insert( tokens.end(), suff.begin(), suff.end() );

  // for( const Token& token : tokens )
  //   fprintf( stderr, "%s|", token.print().c_str() );
  // fprintf( stderr, "\n\n\n" );

  return make_tree( tokens );
}


