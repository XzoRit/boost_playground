#include <boost/sml.hpp>

#include <iostream>
#include <stdio.h>
#include <string_view>

namespace sml = boost::sml;

template <class T>
[[nodiscard]] constexpr auto type_name() -> const char*
{
#if defined(_MSC_VER) and not defined(__clang__)
    return {&__FUNCSIG__[120], sizeof(__FUNCSIG__) - 128};
#elif defined(__clang__)
    return std::string_view{&__PRETTY_FUNCTION__[29], sizeof(__PRETTY_FUNCTION__) - 72}.data();
#elif defined(__GNUC__)
    return {&__PRETTY_FUNCTION__[85], sizeof(__PRETTY_FUNCTION__) - 136};
#endif
}

struct PrintfLogger
{
    template <class SM, class TEvent>
    void log_process_event(const TEvent&)
    {
        printf("[%s][process_event] %s\n", type_name<SM>(), type_name<TEvent>());
    }

    template <class SM, class TGuard, class TEvent>
    void log_guard(const TGuard&, const TEvent&, bool result)
    {
        printf("[%s][guard] %s %s %s\n",
               type_name<SM>(),
               type_name<TGuard>(),
               type_name<TEvent>(),
               (result ? "[OK]" : "[Reject]"));
    }

    template <class SM, class TAction, class TEvent>
    void log_action(const TAction&, const TEvent&)
    {
        printf("[%s][action] %s %s\n", type_name<SM>(), type_name<TAction>(), type_name<TEvent>());
    }

    template <class SM, class TSrcState, class TDstState>
    void log_state_change(const TSrcState& src, const TDstState& dst)
    {
        printf("[%s][transition] %s -> %s\n", type_name<SM>(), src.c_str(), dst.c_str());
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
        std::cout << type_name<T>() << " entry action\n";
    }
};

template<class T>
struct ExitAction
{
    void operator()()
    {
        std::cout << type_name<T>() << " exit action\n";
    }
};

struct Visible
{
    auto operator()() const
    {
        return sml::make_transition_table(*sml::state<Idle> + sml::on_entry<sml::_> / EntryAction<Idle>{},
                                          sml::state<Idle> + sml::on_exit<sml::_> / ExitAction<Idle>{},
                                          sml::state<Idle> + sml::event<Start> / sendStart{} = sml::state<Starting>

                                          ,
                                          sml::state<Starting> + sml::on_entry<sml::_> / EntryAction<Starting>{},
                                          sml::state<Starting> + sml::on_exit<sml::_> / ExitAction<Starting>{},
                                          sml::state<Starting> + sml::event<Run> / sendRunning{} = sml::state<Running>

                                          ,
                                          sml::state<Running> + sml::on_entry<sml::_> / EntryAction<Running>{},
                                          sml::state<Running> + sml::on_exit<sml::_> / ExitAction<Running>{},
                                          sml::state<Running> + sml::event<Finished> / sendFinished{} =
                                              sml::state<Idle>);
    }
};

struct Controller
{
    auto operator()() const
    {
        return sml::make_transition_table(*sml::state<Hidden> + sml::on_entry<sml::_> / EntryAction<Hidden>{},
                                          sml::state<Hidden> + sml::on_exit<sml::_> / ExitAction<Hidden>{},
                                          sml::state<Hidden> + sml::event<Show> = sml::state<Visible>,
                                          sml::state<Hidden> + sml::event<Start> = sml::state<Starting>

                                          ,
                                          sml::state<Visible> + sml::on_entry<sml::_> / EntryAction<Visible>{},
                                          sml::state<Visible> + sml::on_exit<sml::_> / ExitAction<Visible>{},
                                          sml::state<Visible> + sml::event<Hide> = sml::state<Hidden>);
    }
};

#include <cassert>

template <class T>
void dump_transition() noexcept
{
    auto src_state = type_name<typename T::src_state>();
    auto dst_state = type_name<typename T::dst_state>();
    if (std::string{dst_state} == "X")
    {
        dst_state = "[*]";
    }

    if (T::initial)
    {
        std::cout << "[*] --> " << src_state << std::endl;
    }

    std::cout << src_state << " --> " << dst_state;

    const auto has_event = !sml::aux::is_same<typename T::event, sml::anonymous>::value;
    const auto has_guard = !sml::aux::is_same<typename T::guard, sml::front::always>::value;
    const auto has_action = !sml::aux::is_same<typename T::action, sml::front::none>::value;

    if (has_event || has_guard || has_action)
    {
        std::cout << " :";
    }

    if (has_event)
    {
        std::cout << " " << type_name<typename T::event>();
    }

    if (has_guard)
    {
        std::cout << " [" << type_name<typename T::guard>() << "]";
    }

    if (has_action)
    {
        std::cout << " / " << type_name<typename T::action>();
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
    PrintfLogger l;
    sml::sm<Controller, sml::logger<PrintfLogger>> sm{l};
    dump(sm);

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

    sm.process_event(Hide{});
    assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Hidden>));
    assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));

    sm.process_event(Start{});
    // assert(sm.is<decltype(sml::state<Controller>)>(sml::state<Visible>));
    // assert(sm.is<decltype(sml::state<Visible>)>(sml::state<Idle>));
}
