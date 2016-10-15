#include <boost/di.hpp>
#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

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
{
public:
  model(int size, double precision)
  {}

  // boost.di will pick the longest ctor by default
  // this can be achived by this declaration
  // model(int rows, int cols, ...)
  // or by explicitly marking the ctor to be
  // chosen by boost.di
  BOOST_DI_INJECT(model, int rows, int cols)
  {}
};

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

class iclient
{
public:
    virtual void process() = 0;
};

class user : public iclient
{
public:
    void process() override
    {
        cout << "user::process()\n";
    }
};

class timer : public iclient
{
public:
    timer()
    {
        id = Id++;
    }

    void process() override
    {
        cout << "timer::process() with id " << id << '\n';
    }
private:
    static int Id;
    int id;
};

int timer::Id{0};

class app
{
public:
    using clients = vector<shared_ptr<iclient>>;
    app(const string& n, const clients& cs, controller& c)
        : ctrl(c)
        , cls(cs)
        , name(n)
    {
    }

    void run()
    {
        cout << name << " app::run()\n";
        for(auto& c : cls) c->process();
        ctrl.run();
    }
private:
    controller& ctrl;
    const clients& cls;
    string name;
};

int main()
{
    /* iview impl chosen at compile time
    auto injector =
        di::make_injector(
            di::bind<iview>.to<gui_view>(),
            di::bind<int>.to(42),
            di::bind<string>.to("Hello!"),
            di::bind<iclient*[]>().to<user, timer>());
    */
    /* iview impl chosen at runtime */
    const auto use_gui_view = true;
    auto injector =
        di::make_injector(
            di::bind<iview>.to([&](const auto& injector) -> iview&
    {
        if(use_gui_view) return injector.template create<gui_view&>();
        else return injector.template create<text_view&>();
    }),
    di::bind<timer>().in(di::unique),
    // each time a timer is requested
    // a new instance is created
    di::bind<iclient*[]>().to<timer, user, timer>(),
    di::bind<int>.to(42),
    // di::bind<int>.to(123)[di::override],
    di::bind<string>.to("Hello!"));

    auto a = injector.create<app>();

    a.run();

    return 0;
}
