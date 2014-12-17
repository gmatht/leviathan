#include "formula.hpp"
#include "visitor.hpp"

namespace LTL
{

namespace detail
{

#define MAKE_UNARY(_Type, _type)                     \
        _Type##Ptr make_##_type(const FormulaPtr& f) \
        {                                            \
                return std::make_shared<_Type>(f);   \
        }

#define MAKE_BINARY(_Type, _type)                                           \
        _Type##Ptr make_##_type(const FormulaPtr& f1, const FormulaPtr& f2) \
        {                                                                   \
                return std::make_shared<_Type>(f1, f2);                     \
        }

#define ACCEPT_VISITOR(_Type)                \
        void _Type::accept(Visitor& v) const \
        {                                    \
                v.visit(this);               \
        }

TruePtr make_true()
{
        return std::make_shared<True>();
}

FalsePtr make_false()
{
        return std::make_shared<False>();
}

AtomPtr make_atom(const std::string& name)
{
        return std::make_shared<Atom>(name);
}

MAKE_UNARY(Negation, negation)
MAKE_UNARY(Tomorrow, tomorrow)
MAKE_UNARY(Always, always)
MAKE_UNARY(Eventually, eventually)

MAKE_BINARY(Conjunction, conjunction)
MAKE_BINARY(Disjunction, disjunction)
MAKE_BINARY(Then, then)
MAKE_BINARY(Iff, iff)
MAKE_BINARY(Until, until)

ACCEPT_VISITOR(True)
ACCEPT_VISITOR(False)
ACCEPT_VISITOR(Atom)
ACCEPT_VISITOR(Negation)
ACCEPT_VISITOR(Tomorrow)
ACCEPT_VISITOR(Always)
ACCEPT_VISITOR(Eventually)
ACCEPT_VISITOR(Conjunction)
ACCEPT_VISITOR(Disjunction)
ACCEPT_VISITOR(Then)
ACCEPT_VISITOR(Iff)
ACCEPT_VISITOR(Until)

}
}
