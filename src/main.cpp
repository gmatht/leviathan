#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <tuple>
#include <limits>

using namespace std::chrono;

#include "tclap/CmdLine.h"
#include "leviathan.hpp"
#include "json_output.hpp"

using Clock = high_resolution_clock;
const std::string leviathan_version = "0.2.2";

std::vector<std::string> readFile(std::istream &input);
void readableOutput(std::string const&f, bool modelFlag);
void parsableOutput(std::string const&f);

std::vector<std::string> readFile(std::istream& input) {
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(input, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        
        lines.push_back(line);
    }
    
    return lines;
}

void readableOutput(std::string const&f, bool modelFlag, uint64_t depth, uint32_t backtrack_probability, uint32_t min_backtrack, uint32_t max_backtrack)
{
    LTL::PrettyPrinter printer;
    std::cout << "Parsing formula" << std::endl;
    
    LTL::FormulaPtr formula = nullptr;
    bool error = false;
    
    auto p1 = Clock::now();
    std::tie(error, formula) = LTL::parse(f);
    auto p2 = Clock::now();
    
    if (!error)
        std::cout << "Parsing time:  " << duration_cast<microseconds>(p2 - p1).count() << " us" << std::endl;
    else
    {
        std::cout << "Syntax error! Skipping formula: " << f << std::endl;
        return;
    }

    LTL::Solver solver(formula, LTL::FrameID(depth), backtrack_probability, min_backtrack, max_backtrack);
    std::cout << "Checking satisfiability..." << std::endl;
    auto t1 = Clock::now();
    solver.solution();
    auto t2 = Clock::now();

    std::cout << "Is satisfiable: ";
    if (solver.satisfiability() == LTL::Solver::Result::SATISFIABLE)
            std::cout << "True" << std::endl;
    else
            std::cout << "False" << std::endl;

    std::cout << "Time taken: ";
    auto time = (t2 - t1).count();
    if (time > 5000000000)
        std::cout << duration_cast<seconds>(t2 - t1).count() << " sec" << std::endl;
    else if (time > 5000000)
        std::cout << duration_cast<milliseconds>(t2 - t1).count() << " ms" << std::endl;
    else
        std::cout << duration_cast<microseconds>(t2 - t1).count() << " us" << std::endl;

    if (modelFlag && solver.satisfiability() == LTL::Solver::Result::SATISFIABLE)
    {
            LTL::ModelPtr model = solver.model();

            uint64_t i = 0;
            for (auto& state : model->states)
            {
                    std::cout << "State " << i << ":" << std::endl;
                    for (auto& lit : state)
                            std::cout << (lit.positive() ? "" : "\u00AC") << lit.atomic_formula() << ", ";
                    std::cout << std::endl;
                    ++i;
            }

            std::cout << "The model is looping to state " << model->loop_state << std::endl;
    }
    
    std::cout << std::endl;
}

void parsableOutput(std::string const&f) {
    jsonOutput(f);
}

int main(int argc, char* argv[])
{
    TCLAP::CmdLine cmd("A simple LTL satisfiability checker", ' ', leviathan_version);
    TCLAP::ValueArg<std::string> filenameArg("f", "filename", "The name of the file to load the formulas from",
                                             false, "test", "string", cmd);
    TCLAP::ValueArg<std::string> ltlArg("l", "ltl", "The formula to test", false, "p && Xq", "string", cmd);
    
    TCLAP::SwitchArg modelArg("m", "model", "Generates and prints a model of the formula, when satisfiable",
                              cmd, false);
    TCLAP::SwitchArg parsableArg("p", "parsable", "Generates machine-parsable output. It implies -m",
                                 cmd, false);

    TCLAP::ValueArg<uint64_t> depthArg("", "maximum-depth", "The maximum depth to descend into the tableaux (aka the maximum size of the model)", false, std::numeric_limits<uint64_t>::max(), "uint64_t", cmd);
    TCLAP::ValueArg<uint32_t> backtrackPropArg("", "backtrack-probability", "The probability of doing a complete backtrack of the tableaux to check the LOOP and PRUNE rules (between 0 and 100)", false, 100, "uint32_t", cmd);
    TCLAP::ValueArg<uint32_t> minBacktrackArg("", "min-backtrack", "The minimum percentage of the tableaux depth to backtrack during the check of LOOP and PRUNE rules (between 0 and 100)", false, 100, "uint32_t", cmd);
    TCLAP::ValueArg<uint32_t> maxBacktrackArg("", "max-backtrack", "The maximum percentage of the tableaux depth to backtrack during the check of LOOP and PRUNE rules (between 0 and 100)", false, 100, "uint32_t", cmd);

    cmd.parse(argc, argv);

    std::vector<std::string> formulas;
    
    if (filenameArg.isSet() && filenameArg.getValue() != "-") {
        std::ifstream file(filenameArg.getValue(), std::ios::in);
        if (!file.is_open())
        {
            std::cout << "File not found!" << std::endl;
            return 1;
        }
        
        formulas = readFile(file);
    }
    
    if(!ltlArg.isSet() && (!filenameArg.isSet() || filenameArg.getValue() == "-"))
        formulas = readFile(std::cin);
    
    if (ltlArg.isSet())
        formulas.push_back(ltlArg.getValue());
    
    bool parsableFlag = parsableArg.getValue();
    bool modelFlag = parsableFlag || modelArg.getValue();
    
    std::cout << std::boolalpha << std::endl;
    
    for (const auto& f : formulas)
    {
        if(parsableFlag)
            parsableOutput(f);
        else
            readableOutput(f, modelFlag, depthArg.getValue(), backtrackPropArg.getValue(), minBacktrackArg.getValue(), maxBacktrackArg.getValue()); // For some definition of "readable"
    }
    
    //std::this_thread::sleep_until(time_point<system_clock>::max());
    return 0;
}
