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
    void operator()(){}
};

struct sendRunning
{
    void operator()(){}
};

struct sendFinished
{
    void operator()(){}
};

class Hidden;
class Visible;
class Idle;
class Starting;
class Running;

struct Visible
{
    auto operator()() const
    {
        return
            sml::make_transition_table
            (
                *sml::state<Idle> + sml::event<Start> / sendStart{} = sml::state<Starting>
                ,sml::state<Starting> + sml::event<Run> / sendRunning{} = sml::state<Running>
                ,sml::state<Running> + sml::event<Finished> / sendFinished{} = sml::state<Idle>
            );
    }
};

struct Controller
{
    auto operator()() const
    {
        return
            sml::make_transition_table
            (
                *sml::state<Hidden> + sml::event<Show> = sml::state<Visible>
                ,sml::state<Visible> + sml::event<Hide> = sml::state<Hidden>
            );
    }
};

#include <cassert>

int main()
{
    PrintfLogger l;
    sml::sm<Controller, sml::logger<PrintfLogger>> sm{l};
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Hidden>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));

    sm.process_event(Show{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));

    sm.process_event(Start{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Starting>));

    sm.process_event(Run{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Running>));

    sm.process_event(Finished{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));

    sm.process_event(Hide{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Hidden>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));

    sm.process_event(Show{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));

    sm.process_event(Start{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Starting>));

    sm.process_event(Run{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Running>));

    sm.process_event(Hide{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Hidden>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Running>));

    sm.process_event(Show{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));
}
