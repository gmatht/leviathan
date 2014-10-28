//
//  jsonOutput.cpp
//  leviathan
//
//  Created by Nicola Gigante on 28/10/14.
//
//

#include "jsonOutput.hpp"

#include <vector>
#include <chrono>
#include <tuple>
#include <sstream>

#include "leviathan.hpp"

using Clock = std::chrono::high_resolution_clock;

struct JsonOutput
{
    JsonOutput(bool parseError,
               std::vector<LTL::FormulaSet> const&model = {},
               uint64_t loopTo = 0,
               std::chrono::seconds solveTime = {})
    : _parseError(parseError), _model(std::move(model)), _loopTo(loopTo),
      _solveTime(solveTime) { }
    
    void print(std::ostream &out) {
        _output = &out;
        block_t block(*this);
            
        keyvalue("status", _parseError ? "ParseError" : "Valid");
        keyvalue("time", _solveTime.count());
        
        keyblock("model", [&]() {
            output() << indent() << "Ti piacerebbe\n";
        });
    }
    
private:
    bool _parseError;
    std::vector<LTL::FormulaSet> const&_model;
    uint64_t _loopTo;
    std::chrono::seconds _solveTime;
    
    int _indent = 0;
    std::ostream *_output = nullptr;

    std::ostream &output() const {
        return *_output;
    }
    
    std::string indent() const {
        std::string indent;
        
        for(int i = 0; i < _indent; ++i)
            indent += "  ";
        
        return indent;
    }
    
    // This doesn't escape nothing, pay attention
    std::string quoted(std::string const&str) const {
        return ('"' + str + '"');
    }
    
    template<typename T,
    typename std::enable_if<!std::is_same<T, std::string>::value, int>::type = 0>
    std::string quoted(T&& v) const
    {
        std::stringstream str;
        
        str << std::forward<T>(v);
        
        return str.str();
    }
    
    
    template<typename T>
    void keyvalue(std::string const&key,
                  T&& value, bool last = false) const
    {
        output() <<  indent() + quoted(key) + " = " + quoted(value) +
                    (last ? "" : ",") + "\n";
    }
    
    template<typename F>
    void keyblock(std::string const&key, F func) {
        output() << indent() + quoted(key) + " = ";
        
        {
            block_t block(*this);
            func();
        }
    }
    
    struct block_t {
        block_t(JsonOutput &printer) : _printer(printer)
        {
            _printer.output() << "{\n";
            _printer._indent += 1;
        }
        
        ~block_t() {
            _printer._indent -= 1;
            _printer.output() << _printer.indent() << "}\n";
        }
        
    private:
        JsonOutput &_printer;
    };
};

void jsonOutput(std::string const&f)
{
    bool parseError = false;
    LTL::FormulaPtr formula;
    std::vector<LTL::FormulaSet> model;
    uint64_t loopTo = 0;
    std::chrono::seconds solveTime;
    
    std::tie(parseError, formula) = LTL::parse(f);
    
    if(parseError)
        return JsonOutput(true).print(std::cout);
    
    auto t1 = Clock::now();
    std::tie(std::ignore, model, loopTo) = LTL::is_satisfiable(formula, true);
    auto t2 = Clock::now();
    
    solveTime = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
    
    JsonOutput(false, model, loopTo, solveTime).print(std::cout);
}

