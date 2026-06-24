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

// tot felul de chestii
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

int main( int argc, char *argv[] ) {
  assert( argc == 1 + 1 );
  
  Node* root = parse_html( read_file( argv[1] ) );

  dump( root );
  delete root;

  return 0;
}






