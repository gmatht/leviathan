#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <tuple>

using namespace std::chrono;

#include "tclap/CmdLine.h"
#include "leviathan.hpp"
#include "jsonOutput.hpp"

using Clock = high_resolution_clock;

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

void readableOutput(std::string const&f, bool modelFlag)
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
        std::cout << "Error while parsing! Skipping formula: " << f << std::endl;
        return;
    }
    
    /*
    bool is_sat;
    std::vector<LTL::FormulaSet> model;
    uint64_t loopTo;
    
    std::cout << "Checking satisfiability..." << std::endl;
    
    auto t1 = Clock::now();
    std::tie(is_sat, model, loopTo) = LTL::is_satisfiable(formula, modelFlag);
    auto t2 = Clock::now();
    
    std::cout << "Is satisfiable: " << is_sat << std::endl;

    if (modelFlag && is_sat)
    {
        std::cout << "Model has " << model.size() << " states" << std::endl;
        
        std::cout << "Exhibited model: " << std::endl;
        uint64_t i = 0;
        for (const auto& s : model)
        {
            std::cout << "State " << i << ": " << std::endl;
            
            for (const LTL::FormulaPtr _f : s)
            {
                printer.print(_f);
                std::cout << ", ";
            }
            std::cout << std::endl;
            
            ++i;
        }
        std::cout << "The model is looping to state: " << loopTo << std::endl;
    }
    */

    LTL::Solver solver(formula);
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
    
    std::cout << std::endl;
}

void parsableOutput(std::string const&f) {
    jsonOutput(f);
}

int main(int argc, char* argv[])
{
    TCLAP::CmdLine cmd("A simple LTL satisfiability checker", ' ', "0.1");
    TCLAP::ValueArg<std::string> filenameArg("f", "filename", "The name of the file to load the formulas from",
                                             false, "test", "string", cmd);
    TCLAP::ValueArg<std::string> ltlArg("l", "ltl", "The formula to test", false, "p && Xq", "string", cmd);
    
    TCLAP::SwitchArg modelArg("m", "model", "Generates and prints a model of the formula, when satisfiable",
                              cmd, false);
    TCLAP::SwitchArg parsableArg("p", "parsable", "Generates machine-parsable output. It implies -m",
                                 cmd, false);
    
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
            readableOutput(f, modelFlag); // For some definition of "readable"
    }
    
    //std::this_thread::sleep_until(time_point<system_clock>::max());
    return 0;
}
