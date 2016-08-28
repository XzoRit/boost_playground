#include "calculator.hpp"
#include <boost/tokenizer.hpp>
#include <vector>
#include <algorithm>
#include <numeric>

int Calculator::add(std::string numbers)
{
  typedef boost::char_separator<char> SeparatorType;
  typedef boost::tokenizer<SeparatorType> TokenizerType;

  SeparatorType separator{","};
  TokenizerType separatedNums{numbers, separator};

  std::vector<int> numsAsInts{};
  std::transform
    (
     std::begin(separatedNums),
     std::end(separatedNums),
     std::back_inserter(numsAsInts),
     [](auto i){ return std::stoi(i); }
     );
  return std::accumulate
    (
     std::begin(numsAsInts),
     std::end(numsAsInts),
     0
     );
}
