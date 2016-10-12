#include <boost/di.hpp>
#include <string>
#include <iostream>

using namespace std;
namespace di = boost::di;

struct renderer
{
  int device;
};

class view
{
public:
  view(const std::string& title, const renderer& r)
  {}
};

class model
{};

class controller
{
public:
  controller(model& m, view& v)
  {}
};

class user
{};

class app
{
public:
  app(controller& c, user& u)
  {}

  void run()
  {
    cout << "app::run()\n";
  }
};

int main()
{
  auto injector = di::make_injector();
  auto a = injector.create<app>();

  a.run();
  
  return 0;
}
