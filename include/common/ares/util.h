/**
 * @file util.h
 *
 * @brief
 *
 * @date 3/31/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_UTIL_H
#define ARES_COMMON_UTIL_H

#if __cplusplus
extern "C" {
#endif

#include "util_internal.h"

/**
 * @brief Stringify an expression.
 *
 * Stringifies the input.
 *
 * @param x The value or expression to stringify.
 *
 * @note To stringify a macro, use MACRO_STRINGIFY().
 */
#define STRINGIFY(x) #x

/**
 * @brief Stringifies the macro input.
 *
 * Calls stringify on a macro. This is useful where the macro value needs to be
 * stringified.
 *
 * @param x The macro to stringify.
 */
#define MACRO_STRINGIFY(x) STRINGIFY(x)

/**
 * @brief Mark a variable or argument as unused explicitly.
 *
 * Marks an argument or variable as unused, thus circumventing any linter or
 * compile errors. This is useful for cases where variable are used for debug
 * assertions, but aren't used anywhere else. It can also be used in cases where
 * the signature of a function is forced to be a certain thing, but some of the
 * parameters are not used.
 *
 * @param x The argument or variable to mark is unused.
 */
#define ARG_UNUSED(x) (void)x

/**
 * @brief Like COND_CODE_1() except tests if @p _flag is 0.
 *
 * This is like COND_CODE_1(), except that it tests whether @p _flag
 * expands to the integer literal 0. It expands to @p _if_0_code if
 * so, and @p _else_code otherwise; both of these must be enclosed in
 * parentheses.
 *
 * @param _flag evaluated flag
 * @param _if_0_code result if @p _flag expands to 0; must be in parentheses
 * @param _else_code result otherwise; must be in parentheses
 * @see COND_CODE_1()
 */
#define COND_CODE_0(_flag, _if_0_code, _else_code)                             \
    Z_COND_CODE_0(_flag, _if_0_code, _else_code)

/**
 * @brief Insert code depending on whether @p _flag expands to 1 or not.
 *
 * This relies on similar tricks as IS_ENABLED(), but as the result of
 * @p _flag expansion, results in either @p _if_1_code or @p
 * _else_code is expanded.
 *
 * To prevent the preprocessor from treating commas as argument
 * separators, the @p _if_1_code and @p _else_code expressions must be
 * inside brackets/parentheses: <tt>()</tt>. These are stripped away
 * during macro expansion.
 *
 * Example:
 *
 *     COND_CODE_1(CONFIG_FLAG, (uint32_t x;), (there_is_no_flag();))
 *
 * If @p CONFIG_FLAG is defined to 1, this expands to:
 *
 *     uint32_t x;
 *
 * It expands to <tt>there_is_no_flag();</tt> otherwise.
 *
 * This could be used as an alternative to:
 *
 *     #if defined(CONFIG_FLAG) && (CONFIG_FLAG == 1)
 *     #define MAYBE_DECLARE(x) uint32_t x
 *     #else
 *     #define MAYBE_DECLARE(x) there_is_no_flag()
 *     #endif
 *
 *     MAYBE_DECLARE(x);
 *
 * However, the advantage of COND_CODE_1() is that code is resolved in
 * place where it is used, while the @p \#if method defines @p
 * MAYBE_DECLARE on two lines and requires it to be invoked again on a
 * separate line. This makes COND_CODE_1() more concise and also
 * sometimes more useful when used within another macro's expansion.
 *
 * @note @p _flag can be the result of preprocessor expansion, e.g.
 *	 an expression involving <tt>NUM_VA_ARGS_LESS_1(...)</tt>.
 *	 However, @p _if_1_code is only expanded if @p _flag expands
 *	 to the integer literal 1. Integer expressions that evaluate
 *	 to 1, e.g. after doing some arithmetic, will not work.
 *
 * @param _flag evaluated flag
 * @param _if_1_code result if @p _flag expands to 1; must be in parentheses
 * @param _else_code result otherwise; must be in parentheses
 */
#define COND_CODE_1(_flag, _if_1_code, _else_code)                             \
    Z_COND_CODE_1(_flag, _if_1_code, _else_code)

/**
 * @brief Check if a macro has a replacement expression
 *
 * If @p a is a macro defined to a nonempty value, this will return
 * true, otherwise it will return false. It only works with defined
 * macros, so an additional @p \#ifdef test may be needed in some cases.
 *
 * This macro may be used with COND_CODE_1() and COND_CODE_0() while
 * processing `__VA_ARGS__` to avoid processing empty arguments.
 *
 * Example:
 * ```c
 *	#define EMPTY
 *	#define NON_EMPTY	1
 *	#undef  UNDEFINED
 *	IS_EMPTY(EMPTY)
 *	IS_EMPTY(NON_EMPTY)
 *	IS_EMPTY(UNDEFINED)
 *	#if defined(EMPTY) && IS_EMPTY(EMPTY) == true
 *	some_conditional_code
 *	#endif
 *	```
 *
 * In above examples, the invocations of IS_EMPTY(...) return @p true,
 * @p false, and @p true; @p some_conditional_code is included.
 *
 * @param ... macro to check for emptiness (may be `__VA_ARGS__`)
 */
#define IS_EMPTY(...) Z_IS_EMPTY_(__VA_ARGS__)

#if __cplusplus
}
#endif

#endif // ARES_COMMON_UTIL_H
