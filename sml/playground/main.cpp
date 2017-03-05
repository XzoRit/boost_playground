#include <boost/sml.hpp>
#include <typeinfo>
#include <iostream>
#include <stdio.h>

namespace sml = boost::sml;

struct PrintfLogger
{
    template <class SM, class TEvent>
    void log_process_event(const TEvent&)
    {
        printf("[%s][process_event] %s\n", typeid(SM).name(), typeid(TEvent).name());
    }

    template <class SM, class TGuard, class TEvent>
    void log_guard(const TGuard&, const TEvent&, bool result)
    {
        printf("[%s][guard] %s %s %s\n", typeid(SM).name(), typeid(TGuard).name(),
               typeid(TEvent).name(),
               (result ? "[OK]" : "[Reject]"));
    }

    template <class SM, class TAction, class TEvent>
    void log_action(const TAction&, const TEvent&)
    {
        printf("[%s][action] %s %s\n", typeid(SM).name(), typeid(TAction).name(),
               typeid(TEvent).name());
    }

    template <class SM, class TSrcState, class TDstState>
    void log_state_change(const TSrcState& src, const TDstState& dst)
    {
        printf("[%s][transition] %s -> %s\n", typeid(SM).name(), src.c_str(),
               dst.c_str());
    }
};

struct Show {};
struct Hide {};
struct Start {};
struct Run {};
struct Finished {};

struct sendStart
{
    void operator()(const Start&){}
};

struct sendRunning
{
    void operator()(const Run&){}
};

struct sendFinished
{
    void operator()(const Finished&){}
};

class Hidden{};
class Idle{};
class Starting{};
class Running{};

template<class T>
struct EntryAction
{
    void operator()()
    {
        std::cout << typeid(T).name() << " entry action\n";
    }
};

template<class T>
struct ExitAction
{
    void operator()()
    {
        std::cout << typeid(T).name() << " exit action\n";
    }
};

struct Visible
{
    auto operator()() const
    {
        using namespace sml;
        return
            make_transition_table
            (
                *state<Idle> + event<Start> / sendStart{} = state<Starting>
                ,state<Idle> + on_entry<_> / EntryAction<Idle>{}
                ,state<Idle> + on_exit<_> / ExitAction<Idle>{}
                ,state<Starting> + event<Run> / sendRunning{} = state<Running>
                ,state<Starting> + on_entry<_> / EntryAction<Starting>{}
                ,state<Starting> + on_exit<_> / ExitAction<Starting>{}
                ,state<Running> + event<Finished> / sendFinished{} = state<Idle>
                ,state<Running> + on_entry<_> / EntryAction<Running>{}
                ,state<Running> + on_exit<_> / ExitAction<Running>{}
            );
    }
};

struct Controller
{
    auto operator()() const
    {
        using namespace sml;
        return
            make_transition_table
            (
                *state<Hidden> + event<Show> = state<Visible>
                ,state<Hidden> + on_entry<_> / EntryAction<Hidden>{}
                ,state<Hidden> + on_exit<_> / ExitAction<Hidden>{}
                ,state<Visible> + event<Hide> = state<Hidden>
                ,state<Visible> + on_entry<_> / EntryAction<Visible>{}
                ,state<Visible> + on_exit<_> / ExitAction<Visible>{}
            );
    }
};

#include <cassert>

int main()
{
    using namespace sml;
    PrintfLogger l;
    sm<Controller, logger<PrintfLogger>> sm{l};
    assert(sm.is<decltype(state<Controller>)>(state<Hidden>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));

    sm.process_event(Show{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));

    sm.process_event(Start{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Starting>));

    sm.process_event(Run{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Running>));

    sm.process_event(Finished{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));

    sm.process_event(Hide{});
    assert(sm.is<decltype(state<Controller>)>(state<Hidden>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));

    sm.process_event(Show{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));

    sm.process_event(Start{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Starting>));

    sm.process_event(Run{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Running>));

    sm.process_event(Hide{});
    assert(sm.is<decltype(state<Controller>)>(state<Hidden>));
    assert(sm.is<decltype(state<Visible>)>(state<Running>));

    sm.process_event(Show{});
    assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));
}
