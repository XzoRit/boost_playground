#define BOOST_COROUTINES_NO_DEPRECATION_WARNING
#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <functional>
#include <vector>

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

template<class T>
struct Coro
{
public:
  typedef typename bc::asymmetric_coroutine<T>::pull_type PullType;
  typedef typename bc::asymmetric_coroutine<T>::push_type PushType;

  typedef std::vector<T> Container;
  typedef typename Container::const_iterator ConstIter;

  static void next(PushType& sink, const std::vector<T>& data)
  {
    for(ConstIter it = data.begin();  it != data.end(); ++ it)
    {
      sink(*it);
    }
  }
};

int main()
{
  cout << "start of coroutine playground\n\n";

  for(pull_type::iterator it = bc::begin(rangex);
      it !=  bc::end(rangex);
      ++it)
  {
    cout << (*it) << '\n';
  }

  typedef Coro<std::string> CoroString;
  CoroString::Container strings;
  strings.push_back("eins");
  strings.push_back("zwei");
  strings.push_back("drei");

  {
    CoroString::PullType pullStrings(
      boost::bind(CoroString::next, ::_1, boost::cref(strings)));
    for(CoroString::PullType::iterator it = boost::begin(pullStrings);
        it != boost::end(pullStrings);
        ++it)
    {
      cout << (*it) << '\n';
    }
    if(pullStrings) cout << "data left\n";
    else cout << "no data left\n";
  }
  {
    CoroString::PullType pullStrings(
      boost::bind(CoroString::next, ::_1, boost::cref(strings)));
    while(pullStrings)
    {
      cout << pullStrings.get() << '\n';
      cout << pullStrings.get() << '\n';
      pullStrings();
    }
  }
  cout << "\nend of coroutine playground\n";
  return 0;
}
