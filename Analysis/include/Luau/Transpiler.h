// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "Luau/Location.h"
#include "Luau/ParseOptions.h"
#include "Luau/ParseResult.h"

#include <string>

namespace Luau
{
class AstNode;
class AstStatBlock;

struct TranspileResult
{
    std::string code;
    Location errorLocation;
    std::string parseError; // Nonempty if the transpile failed
};

std::string toString(AstNode* node);
void dump(AstNode* node);

// Never fails on a well-formed AST
std::string transpile(AstStatBlock& ast);
std::string transpileWithTypes(AstStatBlock& block);
std::string transpileWithTypes(AstStatBlock &block, const CstNodeMap& cstNodeMap);

// Only fails when parsing fails
TranspileResult transpile(std::string_view source, ParseOptions options = ParseOptions{}, bool withTypes = false);

// Lua 5.4-compatible transpilation (removes Luau-only syntax and rewrites constructs)
// - Removes type annotations and attributes
// - Rewrites continue to goto labels
// - Rewrites compound assignments (+=, -=, etc.)
// - Rewrites if-else expressions as IIFE
// - Rewrites interpolation strings to concatenations with tostring()
// Only fails when parsing fails
TranspileResult transpileToLua54(std::string_view source, ParseOptions options = ParseOptions{});
std::string transpileToLua54(AstStatBlock& block);
std::string transpileToLua54(AstStatBlock& block, const CstNodeMap& cstNodeMap);

} // namespace Luau
