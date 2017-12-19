#include "readconfig.h"
#include <iostream>
#include <fstream>
using std::cout;

int main()
{
  std::ifstream istr("test.dat");

  Config config;
  while (istr.good()) {
    istr >> config;
    cout << config.name << "\t-\t" << config.value << "\n";
  } // while (istr.good())


  return 0;
} // int main()
