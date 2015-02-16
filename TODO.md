# TODO

- Test if having a dynamic vector for eventualities (with a pair<Formula, Frame>) is better than the fixed size option currently used
- Move compareFunc to global operator< between formulas
- Check the TODOs in the code
- better error handling during parsing
- Logging system
- Implement tests
- Rewrite all the formula operator/function to work on Formula directly instead of FormulaPtr
- Use a Formula Pool Allocator to eliminate the shared_ptr overhead
- Check if using "operator size_t()" in Identifiable is better
