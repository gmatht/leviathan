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

#define UNARY_LESS(_Type)  \
        if (isa<_Type>(a)) \
                return fast_cast<_Type>(a)->formula() < fast_cast<_Type>(b)->formula();

#define BINARY_LESS(_Type)                                    \
        if (isa<_Type>(a))                                    \
        {                                                     \
                const _Type* aPtr = fast_cast<_Type>(a);      \
                const _Type* bPtr = fast_cast<_Type>(b);      \
                                                              \
                if (aPtr->left() < bPtr->left())              \
                        return true;                          \
                if (aPtr->left() == bPtr->left())             \
                        return aPtr->right() < bPtr->right(); \
                return false;                                 \
        }

// TODO: Check the ordering: negation of formulas MUST appear after the formula itself (We actually need to implement this)
bool operator<(const FormulaPtr a, const FormulaPtr b)
{
        if (a->type() == b->type())
        {
                if (isa<True>(a) || isa<False>(a))
                        return false;

                if (isa<Atom>(a))
                {
                        const std::string& aName = fast_cast<Atom>(a)->name();
                        const std::string& bName = fast_cast<Atom>(b)->name();
                        return std::lexicographical_compare(aName.begin(), aName.end(), bName.begin(), bName.end());
                }

                UNARY_LESS(Negation)
                UNARY_LESS(Tomorrow)
                UNARY_LESS(Always)
                UNARY_LESS(Eventually)

                BINARY_LESS(Conjunction)
                BINARY_LESS(Disjunction)
                BINARY_LESS(Then)
                BINARY_LESS(Iff)
                BINARY_LESS(Until)
        }

        // If i'm here the types are different
        return a->type() < b->type();
}

bool operator>(const FormulaPtr a, const FormulaPtr b)
{
        return b < a;
}

bool operator<=(const FormulaPtr a, const FormulaPtr b)
{
        return !(a > b);
}

bool operator>=(const FormulaPtr a, const FormulaPtr b)
{
        return !(a < b);
}

#define UNARY_EQ(_Type)    \
        if (isa<_Type>(a)) \
                return fast_cast<_Type>(a)->formula() == fast_cast<_Type>(b)->formula();

#define BINARY_EQ(_Type)   \
        if (isa<_Type>(a)) \
                return (fast_cast<_Type>(a)->left() == fast_cast<_Type>(b)->left()) && (fast_cast<_Type>(a)->right() == fast_cast<_Type>(b)->right());

bool operator==(const FormulaPtr a, const FormulaPtr b)
{
        if (a->type() != b->type())
                return false;

        if (isa<True>(a) || isa<True>(b))
                return true;

        if (isa<False>(a) || isa<False>(b))
                return true;

        if (isa<Atom>(a))
                return fast_cast<Atom>(a)->name() == fast_cast<Atom>(b)->name();

        UNARY_EQ(Negation)
        UNARY_EQ(Tomorrow)
        UNARY_EQ(Always)
        UNARY_EQ(Eventually)

        BINARY_EQ(Conjunction)
        BINARY_EQ(Disjunction)
        BINARY_EQ(Then)
        BINARY_EQ(Iff)
        BINARY_EQ(Until)

        return false;
}

bool operator!=(const FormulaPtr a, const FormulaPtr b)
{
        return !(a == b);
}

static inline bool check_element(const FormulaSet& a, const FormulaSet& b)
{
        for (const FormulaPtr f : a)
        {
                if (b.find(f) == b.end())
                        return false;
        }

        return true;
}

// Defines subset relation for sets of formulas
bool operator<(const FormulaSet& a, const FormulaSet& b)
{
        if (a.size() > b.size())
                return false;

        return check_element(a, b);
}

// The sets have the same formulas
bool operator==(const FormulaSet& a, const FormulaSet& b)
{
        if (a.size() != b.size())
                return false;

        return check_element(a, b);
}

bool operator!=(const FormulaSet& a, const FormulaSet& b)
{
        return !(a == b);
}
}
}
