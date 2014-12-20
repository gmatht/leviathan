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
#include <cassert>

#include "leviathan.hpp"

using Clock = std::chrono::high_resolution_clock;

struct JsonOutput
{
    JsonOutput(bool parseError,
               std::vector<LTL::FormulaSet> const&model = {},
               uint64_t loopTo = 0,
               std::chrono::milliseconds solveTime = {})
    : _parseError(parseError), _model(std::move(model)), _loopTo(loopTo),
      _solveTime(solveTime) { }
    
    void print(std::ostream &out) {
        std::stringstream sstr;
        LTL::PrettyPrinter ltlPrinter(sstr);
        
        _output = &out;
        block_t block(*this, Brace);
            
        keyvalue("valid", _parseError ? "false" : "true");
        keyvalue("time", _solveTime.count());
        
        keyblock("model", Brace, [&]() {
            keyvalue("size", _model.size());
            keyvalue("loop", _loopTo);
            keyblock("states", Bracket, [&] {
                for(auto const&state : _model) {
                    newline();
                    block_t stateBlock(*this, Bracket, IndentedBlock);
                    for(auto const&literal : state) {
                        newline();
                        {
                            block_t literalBlock(*this, Brace, IndentedBlock);
                            
                            bool truth = true;
                            LTL::Atom const*symbol = LTL::fast_cast<LTL::Atom>(literal);
                            
                            if(LTL::Negation const*neg = LTL::fast_cast<LTL::Negation>(literal)) {
                                truth = false;
                                symbol = LTL::fast_cast<LTL::Atom>(neg->formula());
                            }
                            
                            assert(symbol && "We should have only literals at this point");
                            
                            keyvalue("truth", truth ? "true" : "false");
                            
                            sstr.str("");
                            ltlPrinter.print(symbol);
                            keyvalue("symbol", quoted(sstr.str()));
                        }
                    }
                }
            });
        });
    }
    
private:
    bool _parseError;
    std::vector<LTL::FormulaSet> const&_model;
    uint64_t _loopTo;
    std::chrono::milliseconds _solveTime;
    
    int _indent = 0;
    bool _firstElement = true;
    std::ostream *_output = nullptr;
    
    enum paren_t {
        Paren,
        Bracket,
        Brace
    };
    
    enum IndentBlock {
        IndentedBlock,
        InLineBlock
    };
    
    struct block_t {
        block_t(JsonOutput &printer, paren_t paren,
                IndentBlock indentBlock = InLineBlock)
        : _printer(printer), _paren(paren)
        {
            if(indentBlock == IndentedBlock)
                _printer.output() << _printer.indent();
            
            _printer.output() << (_paren == Paren   ? "(" :
                                  _paren == Bracket ? "[" : "{");
            _printer._indent += 1;
            _printer._firstElement = true;
        }
        
        ~block_t() {
            _printer._indent -= 1;
            _printer.output() << "\n" << _printer.indent()
                              << (_paren == Paren   ? ")" :
                                  _paren == Bracket ? "]" : "}");
        }
        
    private:
        JsonOutput &_printer;
        paren_t _paren;
    };
    
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
    
    void newline() {
        if(!_firstElement)
            output() << ",";
        output() << "\n";
        _firstElement = false;
    }
    
    template<typename T>
    void keyvalue(std::string const&key,
                  T&& value)
    {
        newline();
        output() << indent() + quoted(key) + " = ";
        output() << value;
    }
    
    template<typename F>
    void keyblock(std::string const&key, paren_t paren, F func)
    {
        newline();
        output() << indent() + quoted(key) + " = ";
        
        {
            block_t block(*this, paren);
            func();
        }
    }
    
    
    
    
};

void jsonOutput(std::string const&f)
{
    /*
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
    */
}

