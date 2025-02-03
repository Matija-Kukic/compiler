# Compiler

This is a repo containing a compiler i made for a simplified C language.
It contains a lexical analyzer generator(like lex) for any language, sytnax analyzer generator(like yacc),
and a semantic analyzer and code generator for the mentioned simplified C language.
The instructionset that is generated is frisc and can be run with the provided code.

# To do
- Some functionality to be added to generator.
- Adding a cmake build so it could be tested locally.

## File structure
- GEN : generator and semantic analyzer in the same code
- SEM : only semantic analyzer
- SA : syntax analyzer(with generator)
- LA : lexic ananlyzer(with generator)
