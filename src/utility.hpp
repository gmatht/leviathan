#pragma once

template <class T, class S>
inline const S& Container(const std::stack<T, S>& s)
{
        struct Stack : private std::stack<T, S>
        {
                static const S& Container(const std::stack<T, S>& _s)
                {
                        return _s.*&Stack::c;
                }
        };
        return Stack::Container(s);
}

template <class Cont>
class const_reverse_wrapper
{
        const Cont& container;

public:
        const_reverse_wrapper(const Cont& cont)
                : container(cont)
        {
        }

        inline decltype(container.rbegin()) begin() const
        {
                return container.rbegin();
        }

        inline decltype(container.rend()) end() const
        {
                return container.rend();
        }
};

template <class Cont>
class reverse_wrapper
{
        Cont& container;

public:
        reverse_wrapper(Cont& cont)
                : container(cont)
        {
        }

        inline decltype(container.rbegin()) begin()
        {
                return container.rbegin();
        }
        
        inline decltype(container.rend()) end()
        {
                return container.rend();
        }
};

template <class Cont>
const_reverse_wrapper<Cont> reverse(const Cont& cont)
{
        return const_reverse_wrapper<Cont>(cont);
}

template <class Cont>
reverse_wrapper<Cont> reverse(Cont& cont)
{
        return reverse_wrapper<Cont>(cont);
}
