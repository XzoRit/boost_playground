#include <boost/di.hpp>
#include <string>
#include <iostream>
#include <cassert>

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
    gui_view(std::string t, const renderer& r)
        : title(t)
    {
        assert(r.device == 42);
    }
    void update() override
    {
        cout << "gui_view::update " << title << '\n';
    }
private:
    string title;
};

class text_view: public iview
{
public:
    void update() override
    {
        cout << "text_view::update\n";
    }
};

class model
{};

class controller
{
public:
    controller(model& m, iview& v)
        : view(v)
    {}

    void run()
    {
        view.update();
    }
private:
    iview& view;
};

class user
{};

class app
{
public:
    app(const string& n, controller& c, user& u)
        : ctrl(c)
        , usr(u)
        , name(n)
    {
    }

    void run()
    {
        cout << name << " app::run()\n";
        ctrl.run();
    }
private:
    controller& ctrl;
    user& usr;
    string name;
};

int main()
{
    /* iview impl chosen at compile time
      auto injector =
          di::make_injector(
              di::bind<iview>.to<gui_view>(),
              di::bind<int>.to(42),
              di::bind<string>.to("Hello!"));
    */
    /* iview impl chosen at runtime */
    const auto use_gui_view = false;
    auto injector =
        di::make_injector(
            di::bind<iview>.to([&](const auto& injector) -> iview&
    {
        if(use_gui_view) return injector.template create<gui_view&>();
        else return injector.template create<text_view&>();
    }),
    di::bind<int>.to(42),
    di::bind<string>.to("Hello!"));
    auto a = injector.create<app>();

    a.run();

    return 0;
}
