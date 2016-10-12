#include <boost/di.hpp>
#include <string>
#include <iostream>

using namespace std;
namespace di = boost::di;

struct renderer
{
    int device;
};

class iview
{
public:
    virtual ~iview() noexcept = default;
    virtual void update() = 0;
};

class gui_view: public iview
{
public:
    gui_view(std::string title, const renderer&) {}
    void update() override {}
};

class text_view: public iview
{
public:
    void update() override {}
};

class model
{};

class controller
{
public:
    controller(model& m, iview& v)
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
    auto injector =
        di::make_injector(
            di::bind<iview>.to<gui_view>());
    auto a = injector.create<app>();

    a.run();

    return 0;
}
