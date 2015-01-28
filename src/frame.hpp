#pragma once

#include "boost/dynamic_bitset.hpp"
#include "identifiable.hpp"
#include <unordered_map>
#include <cstdint>
#include <iostream>

namespace LTL
{
namespace detail
{

using Bitset = boost::dynamic_bitset<uint64_t>;

// TODO: Move from an unordered_map-based representation of eventualities to a bitset-based representation
// TODO: Check Frame size and alignment
// TODO: Investigate why move constructor isn't used
struct Frame
{
        Bitset formulas;
        Bitset to_process;
        std::unordered_map<FormulaID, FrameID> eventualities;
        FrameID id;
        FormulaID choosenFormula;
        bool choice;
        Frame* chain;

        // Builds a frame with a single formula in it (represented by the index in the table) -> Start of the process
        Frame(const FrameID _id, const FormulaID _formula, uint64_t number_of_formulas)
                : formulas(number_of_formulas)
                , to_process(number_of_formulas)
                , eventualities()
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
        Frame(const FrameID _id, uint64_t number_of_formulas, const std::unordered_map<FormulaID, FrameID>& _eventualities, Frame* chainPtr)
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

        Frame(const Frame& f) : formulas(f.formulas), to_process(f.to_process), eventualities(f.eventualities), id(f.id), choosenFormula(f.choosenFormula), choice(f.choice), chain(f.chain) {}
        
        Frame(Frame&& f)
        {
                std::swap(formulas, f.formulas);
                std::swap(to_process, f.to_process);
                std::swap(eventualities, f.eventualities);
                id = f.id;
                choosenFormula = f.choosenFormula;
                choice = f.choice;
                chain = f.chain;
        }
};

}
}
