#include <calculator.hpp>
#include <cassert>

int main(int argc, char *argv[])
{
  assert(Calculator::add("1,22,333") == 356);
  
  return 0;
}
