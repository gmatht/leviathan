#pragma once

#include "boost/dynamic_bitset.hpp"
#include "identifiable.hpp"
#include "tag_ptr.hpp"
#include <unordered_map>
#include <cstdint>
#include <iostream>

namespace LTL
{
namespace detail
{

using Bitset = boost::dynamic_bitset<uint64_t>;

enum Eventuality : uint64_t
{
        NOT_REQUESTED = std::numeric_limits<uint64_t>::max(),
        NOT_SATISFIED = std::numeric_limits<uint64_t>::max() - 1
};

// TODO: Move from an unordered_map-based representation of eventualities to a bitset-based representation
struct Frame
{
        enum Type : uint8_t
        {
                STEP = 0,
                CHOICE = 1,
                SAT = 2
        };

        Bitset formulas;
        Bitset to_process;
        std::vector<FrameID> eventualities;
        FrameID id;
        FormulaID choosenFormula;
        bool choice;
        Frame* chain;

        // Builds a frame with a single formula in it (represented by the index in the table) -> Start of the process
        Frame(const FrameID _id, const FormulaID _formula, uint64_t number_of_formulas, uint64_t number_of_eventualities)
                : formulas(number_of_formulas)
                , to_process(number_of_formulas)
                , eventualities(number_of_eventualities)
                , id(_id)
                , choosenFormula(FormulaID::max())
                , choice(false)
                , chain(nullptr)
        {
                formulas.set(_formula);
                to_process.set();
        }

        // Builds a frame with the same formulas of the given frame in it -> Choice point
        Frame(const FrameID _id, const Frame& _frame)
                : formulas(_frame.formulas)
                , to_process(_frame.to_process)
                , eventualities(_frame.eventualities)
                , id(_id)
                , choosenFormula(FormulaID::max())
                , choice(false)
                , chain(_frame.chain)
        {
        }

        // Builds a frame with the given sets of eventualities (needs to be manually filled with the formulas) -> Step rule
        Frame(const FrameID _id, uint64_t number_of_formulas, const std::vector<FrameID>& _eventualities, Frame* chainPtr)
                : formulas(number_of_formulas)
                , to_process(number_of_formulas)
                , eventualities(_eventualities)
                , id(_id)
                , choosenFormula(FormulaID::max())
                , choice(false)
                , chain(chainPtr)
        {
                to_process.set();
        }
};

}
}
