#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <tuple>

using namespace std::chrono;

#include "leviathan.hpp"
#include "tclap/CmdLine.h"

using Clock = high_resolution_clock;

int main(int argc, char* argv[])
{
        TCLAP::CmdLine cmd("A simple LTL satisfiability checker", ' ', "0.1");
        TCLAP::ValueArg<std::string> filenameArg("f", "filename", "The name of the file to load the formulas from",
                                                 true, "test", "string");
        TCLAP::ValueArg<std::string> ltlArg("l", "ltl", "The formula to test", true, "p && Xq", "string");
        cmd.xorAdd(filenameArg, ltlArg);

        TCLAP::SwitchArg modelArg("m", "model", "Generates and prints a model of the formula, when satisfiable", cmd, false);

        cmd.parse(argc, argv);

        std::vector<std::string> formulas;

        if (filenameArg.isSet())
        {
                std::ifstream file(filenameArg.getValue(), std::ios::in);
                if (!file.is_open())
                {
                        std::cout << "File not found!" << std::endl;
                        return 1;
                }

                std::string formula;

                while (std::getline(file, formula))
                        formulas.push_back(formula);
        }
        else if (ltlArg.isSet())
                formulas.push_back(ltlArg.getValue());

        bool modelFlag = modelArg.getValue();

        std::cout << std::boolalpha << std::endl;

        LTL::PrettyPrinter printer;
        for (const auto& f : formulas)
        {

                std::cout << "Parsing formula: " << f << std::endl;

                LTL::FormulaPtr formula = nullptr;
                bool error = false;

                auto p1 = Clock::now();
                std::tie(error, formula) = LTL::parse(f);
                auto p2 = Clock::now();

                if (!error)
                {
                        std::cout << "Parsing result: ";
                        printer.print(formula, true);
                        std::cout << "Parsing time:  " << duration_cast<microseconds>(p2 - p1).count() << " us" << std::endl;
                }
                else
                {
                        std::cout << "Error while parsing! Skipping formula: " << f << std::endl;
                        continue;
                }

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

                std::cout << "Time taken: ";
                auto time = (t2 - t1).count();
                if (time > 1000000000)
                        std::cout << duration_cast<seconds>(t2 - t1).count() << " sec" << std::endl;
                else if (time > 1000000)
                        std::cout << duration_cast<milliseconds>(t2 - t1).count() << " ms" << std::endl;
                else
                        std::cout << duration_cast<microseconds>(t2 - t1).count() << " us" << std::endl;

                std::cout << std::endl;
        }

        //std::this_thread::sleep_until(time_point<system_clock>::max());
        return 0;
}
