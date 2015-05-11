#pragma once

#include "boost/dynamic_bitset.hpp"
#include "boost/pool/pool_alloc.hpp"
#include "identifiable.hpp"
#include "minisat/core/Solver.h"
#include <memory>
#include <cstdint>

namespace LTL
{
namespace detail
{

using Bitset = boost::dynamic_bitset<uint64_t>;

class Eventuality
{
public:
        Eventuality() : _id(NOT_REQUESTED) {}
        Eventuality(FrameID id) : _id(id) {}

        FrameID& id()
        {
                return _id;
        }

        FrameID id() const
        {
                return _id;
        }

        bool is_not_requested() const
        {
                return _id == FrameID(NOT_REQUESTED);
        }

        bool is_not_satisfied() const
        {
                return _id == FrameID(NOT_SATISFIED);
        }

        bool is_satisfied() const
        {
                return _id < FrameID(NOT_SATISFIED);
        }

        void set_not_satisfied()
        {
                _id = FrameID(NOT_SATISFIED);
        }

        void set_satisfied(const FrameID& id)
        {
                _id = id;
        }

private:
        FrameID _id;

        static constexpr uint64_t NOT_REQUESTED = std::numeric_limits<uint64_t>::max();
        static constexpr uint64_t NOT_SATISFIED = std::numeric_limits<uint64_t>::max() - 1;
};

using Eventualities = std::vector<Eventuality, boost::fast_pool_allocator<Eventuality>>;
using Literals = std::vector<int>;

struct Frame
{
        enum Type : uint8_t
        {
                UNKNOWN = 0,
                STEP = 1,
                CHOICE = 2,
                SAT = 3
        };

        Bitset formulas;
        Bitset to_process;
        Eventualities eventualities;
        Literals literals;
        FrameID id;
        FormulaID choosenFormula;
        Frame* chain;
        std::unique_ptr<Minisat::Solver> solver;
        Type type;
        Frame* prev;
        Frame* first;

        // Builds a frame with a single formula in it (represented by the index in the table) -> Start of the process
        Frame(const FrameID _id, const FormulaID _formula, uint64_t number_of_formulas, uint64_t number_of_eventualities)
                : formulas(number_of_formulas)
                , to_process(number_of_formulas)
                , eventualities(number_of_eventualities)
                , literals()
                , id(_id)
                , choosenFormula(FormulaID::max())
                , chain(nullptr)
                , solver(nullptr)
                , type(UNKNOWN)
                , prev(nullptr)
                , first(nullptr)
        {
                formulas.set(_formula);
                to_process.set();
        }

        // Builds a frame with the same formulas of the given frame in it -> Choice point
        Frame(const Frame& _frame)
                : formulas(_frame.formulas)
                , to_process(_frame.to_process)
                , eventualities(_frame.eventualities, _frame.eventualities.get_allocator())
                , literals()
                , id(_frame.id)
                , choosenFormula(FormulaID::max())
                , chain(_frame.chain)
                , solver(nullptr)
                , type(UNKNOWN)
                , prev(nullptr)
                , first(nullptr)
        {
        }

        // Builds a frame with the given sets of eventualities (needs to be manually filled with the formulas) -> Step rule
        Frame(const FrameID _id, uint64_t number_of_formulas, const Eventualities& _eventualities, Frame* chainPtr)
                : formulas(number_of_formulas)
                , to_process(number_of_formulas)
                , eventualities(_eventualities, _eventualities.get_allocator())
                , literals()
                , id(_id)
                , choosenFormula(FormulaID::max())                
                , chain(chainPtr)
                , solver(nullptr)
                , type(UNKNOWN)
                , prev(nullptr)
                , first(nullptr)
        {
                to_process.set();
        }
};

}
}
