# TODO

- Test if having a dynamic vector for eventualities (with a pair<Formula, Frame>) is better than the fixed size option currently used
- Move compareFunc to global operator< between formulas
- Check the TODOs in the code
- better error handling during parsing
- Rewrite all the formula operator/function to work on Formula directly instead of FormulaPtr
- Use a Formula Pool Allocator to eliminate the shared_ptr overhead
- Check if using "operator size_t()" in Identifiable is better
- Treat Not Untils as an explicit formula type since they have always been a special negation case for us?
- Stack trace on crash: http://oroboro.com/stack-trace-on-crash/ (http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes)
- Refactor duplicated code during branch frame initialization? (Inside the main loop and the roolback function)
- Not every field of Frame is needed in every type of Frame, check if something can be compressed with unions like (FormulaID, FrameID) or (Eventualities, Literal vector)
