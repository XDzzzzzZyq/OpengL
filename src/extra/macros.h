/**
 * @file macros.h
 * @brief Utility macros for debugging, iteration, and variadic operations.
 * 
 * This header provides commonly used preprocessor macros for development convenience:
 * - Debug output macros (conditionally compiled in DEBUG builds)
 * - OpenGL error checking and reporting
 * - Loop iteration shortcuts
 * - Variadic macro helpers for argument counting and overloading
 * 
 * Architecture:
 * - Debug macros compile to no-ops in RELEASE builds (zero overhead)
 * - OpenGL debugging helps catch state errors during development
 * - Variadic macros enable template-like macro overloading
 * 
 * Design Principles:
 * - Minimize runtime overhead (most macros are compile-time only)
 * - Debug output only in _DEBUG builds
 * - Use inline functions where possible for type safety
 * 
 * @note All debug macros are no-ops in release builds to eliminate overhead.
 * @note OpenGL error checking is expensive - use sparingly in hot paths.
 */

#pragma once

#ifdef _DEBUG

#include <GL/glew.h>
#include <iostream>
#include <string>

/**
 * @brief Convert OpenGL error code to human-readable string.
 * 
 * Queries glGetError() and returns a descriptive string for the error code.
 * Used by GLDEBUG macro for error reporting with source location.
 * 
 * Error Codes:
 * - GL_NO_ERROR: No error has occurred
 * - GL_INVALID_ENUM: Invalid enum argument
 * - GL_INVALID_VALUE: Numeric argument out of range
 * - GL_INVALID_OPERATION: Operation illegal in current state
 * - GL_INVALID_FRAMEBUFFER_OPERATION: Framebuffer incomplete
 * - GL_OUT_OF_MEMORY: Not enough memory to execute command
 * 
 * @return String representation of current OpenGL error state
 * 
 * @note Calling glGetError() clears the error flag - only call once per check.
 * @note Unknown errors return "UNKNOWN_ERROR" with numeric code.
 */
inline std::string glDebug_xdz() {
	const GLuint err = glGetError();
	switch (err)
	{
#define GL_ERR(err) case GL_##err: return #err
		GL_ERR(NO_ERROR);
		GL_ERR(INVALID_ENUM);
		GL_ERR(INVALID_VALUE);
		GL_ERR(INVALID_OPERATION);
		GL_ERR(INVALID_FRAMEBUFFER_OPERATION);
		GL_ERR(OUT_OF_MEMORY);
	default:
		return "UNKNOWN_ERROR" + std::to_string(err);
	}
}

/**
 * @brief Print debug message to stdout (DEBUG builds only).
 * 
 * Simple debug output for values or messages. Compiles to no-op in release builds.
 * 
 * @param x Expression to output (must support operator<<)
 * 
 * Example:
 * @code
 * DEBUG("Initializing renderer");
 * DEBUG(camera->GetPosition());
 * @endcode
 */
#define DEBUG(x) std::cout<<(x)<<"\n"

/**
 * @brief Print debug message with function name and line number (DEBUG builds only).
 * 
 * Enhanced debug output that includes source location for easier tracing.
 * Useful for tracking execution flow and pinpointing issues.
 * 
 * @param x Expression to output (must support operator<<)
 * 
 * Output Format: `function_name : line_number : value`
 * 
 * Example:
 * @code
 * DEBUGS("Framebuffer created");
 * // Output: "Init : 42 : Framebuffer created"
 * @endcode
 */
#define DEBUGS(x) std::cout<<__func__<<" : "<<__LINE__<<" : "<<(x)<<"\n"

/**
 * @brief Check and print OpenGL error state with source location (DEBUG builds only).
 * 
 * Queries glGetError() and outputs any error with function name and line number.
 * Use after OpenGL calls to catch state errors during development.
 * 
 * Output Format: `function_name : line_number : GL_ERROR_NAME`
 * 
 * @note Clears OpenGL error flag when called.
 * @note Expensive - avoid in tight loops or release builds.
 * 
 * Example:
 * @code
 * glBindFramebuffer(GL_FRAMEBUFFER, fbo);
 * GLDEBUG; // Checks for errors from bind call
 * @endcode
 */
#define GLDEBUG std::cout<<__func__<<" : "<<__LINE__<<" : "<<glDebug_xdz()<<"\n"

#else

/**
 * @brief No-op in release builds (DEBUG stripped).
 */
#define DEBUG(x)

/**
 * @brief No-op in release builds (DEBUGS stripped).
 */
#define DEBUGS(x)

/**
 * @brief No-op in release builds (GLDEBUG stripped).
 */
#define GLDEBUG

#endif


/**
 * @brief Default screen width for initialization.
 * 
 * Initial viewport width used for framebuffer and window creation.
 * Can be overridden at runtime via viewport resize events.
 * 
 * @note This is a legacy constant - prefer reading viewport size from Context.
 */
#define SCREEN_W 1740

/**
 * @brief Default screen height for initialization.
 * 
 * Initial viewport height used for framebuffer and window creation.
 * Can be overridden at runtime via viewport resize events.
 * 
 * @note This is a legacy constant - prefer reading viewport size from Context.
 */
#define SCREEN_H 810

/**
 * @brief Loop from 0 to x-1 with custom iterator name.
 * 
 * Shorthand for range-based integer loop with configurable iterator variable.
 * 
 * @param x Upper bound (exclusive)
 * @param n Iterator variable name
 * 
 * Example:
 * @code
 * LOOP_N(10, j) {
 *     // j goes from 0 to 9
 * }
 * @endcode
 */
#define LOOP_N(x, n) for(int n = 0; n < int(x); n++)

/**
 * @brief Loop from 0 to x-1 using iterator 'i'.
 * 
 * Convenience macro for simple loops with default iterator name.
 * 
 * @param x Upper bound (exclusive)
 * 
 * Example:
 * @code
 * LOOP(vertices.size()) {
 *     // i goes from 0 to vertices.size()-1
 * }
 * @endcode
 */
#define LOOP(x) LOOP_N(x, i)

/**
 * @brief Create std::vector<std::string> from variadic arguments.
 * 
 * Shorthand for initializing string vectors from comma-separated string literals.
 * Commonly used for dropdown options or choice lists in UI.
 * 
 * @param ... Comma-separated string literals
 * 
 * Example:
 * @code
 * auto modes = OPTIONS("Perspective", "Orthographic", "Stereo");
 * // Equivalent to: std::vector<std::string>{"Perspective", "Orthographic", "Stereo"}
 * @endcode
 */
#define OPTIONS(...) std::vector<std::string>{__VA_ARGS__}

/* --- internal helpers --- */

/**
 * @brief Internal macro concatenation helper.
 * 
 * Used by variadic macro system to glue tokens together.
 * 
 * @note Internal use only - not intended for direct invocation.
 */
#define _VA_NARGS_GLUE(x, y) x y

/**
 * @brief Internal argument counter helper.
 * 
 * Returns the count from the 64th position in the argument list.
 * Used by VA_NARGS_COUNT to determine variadic argument count.
 * 
 * @note Internal use only - not intended for direct invocation.
 */
#define _VA_NARGS_RETURN_COUNT(\
  _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, \
  _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, \
  _33_, _34_, _35_, _36_, _37_, _38_, _39_, _40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, \
  _49_, _50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, _60_, _61_, _62_, _63_, _64_, \
  count, ...) count

/**
 * @brief Internal expansion helper for argument counting.
 * 
 * @note Internal use only - not intended for direct invocation.
 */
#define _VA_NARGS_EXPAND(args) _VA_NARGS_RETURN_COUNT args

/**
 * @brief Internal macro overload helper (level 2).
 * 
 * @note Internal use only - not intended for direct invocation.
 */
#define _VA_NARGS_OVERLOAD_MACRO2(name, count) name##count

/**
 * @brief Internal macro overload helper (level 1).
 * 
 * @note Internal use only - not intended for direct invocation.
 */
#define _VA_NARGS_OVERLOAD_MACRO1(name, count) _VA_NARGS_OVERLOAD_MACRO2(name, count)

/**
 * @brief Internal macro overload dispatcher.
 * 
 * @note Internal use only - not intended for direct invocation.
 */
#define _VA_NARGS_OVERLOAD_MACRO(name,  count) _VA_NARGS_OVERLOAD_MACRO1(name, count)

/* --- expose for re-use --- */

/**
 * @brief Count variadic macro arguments at compile-time.
 * 
 * Returns the number of arguments passed to the macro (up to 64).
 * Used by VA_NARGS_CALL_OVERLOAD to dispatch to correct overload.
 * 
 * @param ... Variadic arguments to count
 * @return Integer literal representing argument count
 * 
 * @note Maximum 64 arguments supported.
 * 
 * Example:
 * @code
 * VA_NARGS_COUNT(a, b, c) // Expands to 3
 * VA_NARGS_COUNT(x)       // Expands to 1
 * @endcode
 */
#define VA_NARGS_COUNT(...) _VA_NARGS_EXPAND((__VA_ARGS__, \
  64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
  48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
  32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
  16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2, 1, 0))

/**
 * @brief Call macro overload based on argument count.
 * 
 * Enables compile-time macro overloading by appending argument count to macro name.
 * Used for implementing ISIN and other variadic macros with different behaviors.
 * 
 * @param name Base macro name (overloads must be named name2, name3, etc.)
 * @param ... Variadic arguments to pass to overload
 * 
 * Example:
 * @code
 * // Define overloads:
 * #define PRINT2(a, b) std::cout << a << b
 * #define PRINT3(a, b, c) std::cout << a << b << c
 * 
 * // Use dispatcher:
 * VA_NARGS_CALL_OVERLOAD(PRINT, x, y)    // Calls PRINT2
 * VA_NARGS_CALL_OVERLOAD(PRINT, x, y, z) // Calls PRINT3
 * @endcode
 */
#define VA_NARGS_CALL_OVERLOAD(name, ...) \
  _VA_NARGS_GLUE(_VA_NARGS_OVERLOAD_MACRO(name, VA_NARGS_COUNT(__VA_ARGS__)), (__VA_ARGS__))

/* ISIN#(v, ...): is the first arg equal any others? */

/* internal helpers. */

/**
 * @brief Check if v equals a (2-argument base case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN2(v, a) ((v) == (a))

/**
 * @brief Check if v equals a or b (3-argument case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN3(v, a, b) \
  (_VA_ISIN2(v, a) || _VA_ISIN2(v, b))

/**
 * @brief Check if v equals a, b, or c (4-argument case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN4(v, a, b, c) \
  (_VA_ISIN3(v, a, b) || _VA_ISIN2(v, c))

/**
 * @brief Check if v equals a, b, c, or d (5-argument case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN5(v, a, b, c, d) \
  (_VA_ISIN4(v, a, b, c) || _VA_ISIN2(v, d))

/**
 * @brief Check if v equals a, b, c, d, or e (6-argument case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN6(v, a, b, c, d, e) \
  (_VA_ISIN5(v, a, b, c, d) || _VA_ISIN2(v, e))

/**
 * @brief Check if v equals a, b, c, d, e, or f (7-argument case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN7(v, a, b, c, d, e, f) \
  (_VA_ISIN6(v, a, b, c, d, e) || _VA_ISIN2(v, f))

/**
 * @brief Check if v equals a, b, c, d, e, f, or g (8-argument case).
 * 
 * @note Internal use only - use ISIN macro instead.
 */
#define _VA_ISIN8(v, a, b, c, d, e, f, g) \
  (_VA_ISIN7(v, a, b, c, d, e, f) || _VA_ISIN2(v, g))

/**
 * @brief Check if value equals any of the provided arguments.
 * 
 * Variadic macro that checks if the first argument equals any of the remaining
 * arguments. Automatically selects correct overload based on argument count.
 * 
 * @param v Value to check
 * @param ... Set of values to compare against
 * @return Boolean expression (compile-time evaluated)
 * 
 * @note Supports 2-8 arguments (value + 1-7 comparisons).
 * @note Uses short-circuit evaluation for efficiency.
 * 
 * Example:
 * @code
 * if (ISIN(type, TYPE_MESH, TYPE_CAMERA, TYPE_LIGHT)) {
 *     // Handle renderable object types
 * }
 * @endcode
 */
#define ISIN(...) VA_NARGS_CALL_OVERLOAD(_VA_ISIN, __VA_ARGS__)