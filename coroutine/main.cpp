#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <functional>

using namespace std;
namespace bc = boost::coroutines;
typedef bc::asymmetric_coroutine<int>::pull_type pull_type;
typedef bc::asymmetric_coroutine<int>::push_type push_type;

void range(push_type& sink, int limit)
{
  for(int i = 0; i < limit; ++i)
  {
    sink(i);
  }
}

pull_type rangex(boost::bind(range, ::_1, 17));

int main()
{
  cout << "start of coroutine playground\n";
  for(pull_type::iterator it = bc::begin(rangex);
      it !=  bc::end(rangex);
      ++it)
  {
    cout << (*it) << '\n';
  }
  cout << "\nend of coroutine playground\n";
  return 0;
}
