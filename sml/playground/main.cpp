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
    void operator()(const Start&) {}
};

struct sendRunning
{
    void operator()(const Run&) {}
};

struct sendFinished
{
    void operator()(const Finished&) {}
};

class Hidden {};
class Idle {};
class Starting {};
class Running {};

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
                *state<Idle> + on_entry<_> / EntryAction<Idle>{}
                ,state<Idle> + on_exit <_> / ExitAction <Idle>{}
                ,state<Idle> + event<Start> / sendStart{} = state<Starting>

                ,state<Starting> + on_entry<_> / EntryAction<Starting>{}
                ,state<Starting> + on_exit <_> / ExitAction <Starting>{}
                ,state<Starting> + event<Run> / sendRunning{} = state<Running>

                ,state<Running> + on_entry<_> / EntryAction<Running>{}
                ,state<Running> + on_exit <_> / ExitAction <Running>{}
                ,state<Running> + event<Finished> / sendFinished{} = state<Idle>
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
                *state<Hidden>  + on_entry<_> / EntryAction<Hidden>{}
                ,state<Hidden>  + on_exit <_> / ExitAction <Hidden>{}
                ,state<Hidden>  + event<Show>  = state<Visible>
                ,state<Hidden>  + event<Start> = state<Starting>

                ,state<Visible> + on_entry<_> / EntryAction<Visible>{}
                ,state<Visible> + on_exit <_> / ExitAction <Visible>{}
                ,state<Visible> + event<Hide> = state<Hidden>
            );
    }
};

#include <cassert>

template <class T>
void dump_transition() noexcept
{
    auto src_state = std::string{sml::aux::string<typename T::src_state>{} .c_str()};
    auto dst_state = std::string{sml::aux::string<typename T::dst_state>{} .c_str()};
    if(dst_state == "X")
    {
        dst_state = "[*]";
    }

    if(T::initial)
    {
        std::cout << "[*] --> " << src_state << std::endl;
    }

    std::cout << src_state << " --> " << dst_state;

    const auto has_event =
        !sml::aux::is_same<typename T::event, sml::anonymous>::value;
    const auto has_guard =
        !sml::aux::is_same<typename T::guard, sml::front::always>::value;
    const auto has_action =
        !sml::aux::is_same<typename T::action, sml::front::none>::value;

    if(has_event || has_guard || has_action)
    {
        std::cout << " :";
    }

    if(has_event)
    {
        std::cout << " " << typeid(typename T::event).name();
    }

    if(has_guard)
    {
        std::cout << " [" << typeid(typename T::guard).name() << "]";
    }

    if(has_action)
    {
        std::cout << " / " << typeid(typename T::action).name();
    }

    std::cout << std::endl;
}

template <template <class...> class T, class... Ts>
void dump_transitions(const T<Ts...>&) noexcept
{
    int _[] = {0, (dump_transition<Ts>(), 0)...};
    (void)_;
}

template <class SM>
void dump(const SM&) noexcept
{
    std::cout << "@startuml" << std::endl << std::endl;
    dump_transitions(typename SM::transitions{});
    std::cout << std::endl << "@enduml" << std::endl;
}

int main()
{
    using namespace sml;
    PrintfLogger l;
    sm<Controller, logger<PrintfLogger>> sm{l};
    dump(sm);

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

    sm.process_event(Hide{});
    assert(sm.is<decltype(state<Controller>)>(state<Hidden>));
    assert(sm.is<decltype(state<Visible>)>(state<Idle>));

    sm.process_event(Start{});
    // assert(sm.is<decltype(state<Controller>)>(state<Visible>));
    // assert(sm.is<decltype(state<Visible>)>(state<Idle>));

}
