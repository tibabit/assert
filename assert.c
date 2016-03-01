#include "assert.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "runner.h"
#include "spec_reporter.h"

FILE *dev_null = NULL;

#define start_clock()               time_t start_time = clock()
#define stop_clock()                time_t end_time = clock()
#define get_elapsed_time()          (int)((double)(end_time - start_time) / CLOCKS_PER_SEC * 1000 + 0.5)

#define bool_to_str(val)            (val == true ? "true" : "false")


/* COMPARISON FUNCTIONS */
#define comparator(actual, expected, operator)              (actual operator expected)
#define comparator_boolp(actual, expected, operator)        (strcmp(actual, expected) operator 0); //comparator(actual, expected, operator)
#define comparator_char(actual, expected, operator)         comparator(actual, expected, operator)
#define comparator_short(actual, expected, operator)        comparator(actual, expected, operator)
#define comparator_int(actual, expected, operator)          comparator(actual, expected, operator)
#define comparator_long(actual, expected, operator)         comparator(actual, expected, operator)
#define comparator_float(actual, expected, operator)        comparator(actual, expected, operator)
#define comparator_double(actual, expected, operator)       comparator(actual, expected, operator)
#define comparator_string(actual, expected, operator)       (strcmp(actual, expected) operator 0)

#define comparer(suffix, type)                                              \
    bool_t comparer_equal_##suffix(type actual, type expected)             \
    {                                                                       \
        return comparator_##suffix(actual, expected, ==);                   \
    }                                                                       \
    bool_t comparer_above_##suffix(type actual, type expected)             \
    {                                                                       \
        return comparator_##suffix(actual, expected, >);                    \
    }                                                                       \
    bool_t comparer_above_or_equal_##suffix(type actual, type expected)    \
    {                                                                       \
        return comparator_##suffix(actual, expected, >=);                   \
    }                                                                       \
    bool_t comparer_below_##suffix(type actual, type expected)             \
    {                                                                       \
        return comparator_##suffix(actual, expected, <);                    \
    }                                                                       \
    bool_t comparer_below_or_equal_##suffix(type actual, type expected)    \
    {                                                                       \
        return comparator_##suffix(actual, expected, <=);                   \
    }

#define executor_operator_definition(suffix, type)                                                            \
    bool_t executor_operator_##suffix(type actual, type expected, operator_t operator)                       \
    {                                                                                                       \
            return operator == operator_equal ? comparer_equal_##suffix(actual, expected)                     \
            : (operator == operator_above ? comparer_above_##suffix(actual, expected)                         \
            : (operator == operator_above_or_equal ? comparer_above_or_equal_##suffix(actual, expected)       \
            : (operator == operator_below ? comparer_below_##suffix(actual, expected)                         \
            : comparer_below_or_equal_##suffix(actual, expected))));                                          \
    }

comparer(boolp, string_t)
comparer(char, char)
comparer(short, short)
comparer(int, int)
comparer(long, long)
comparer(float, float)
comparer(double, double)
comparer(string, string_t)

executor_operator_definition(boolp, string_t)
executor_operator_definition(char, char)
executor_operator_definition(short, short)
executor_operator_definition(int, int)
executor_operator_definition(long, long)
executor_operator_definition(float, float)
executor_operator_definition(double, double)
executor_operator_definition(string, string_t)

/* test executor function */
#define executor_definition(suffix, type, format)                                   \
    void executor_##suffix(string_t file,                                             \
        line_t ln,                                                                    \
        type actual,                                                                \
        bool_t output,                                                             \
        type expected,                                                              \
        operator_t operator)                                                        \
    {                                                                               \
        bool_t result = executor_operator_##suffix(actual, expected, operator);    \
        bool_t is_failure = output ? !(result) : (result);                         \
        if (is_failure)                                                             \
        {                                                                           \
            int message_length = 0;                                                 \
            assertion_t * assertion = assertion_create(file, ln, "Failed");              \
            test_t * test = test_current(g_test_runner);                                           \
            test_add_failure(test, assertion);                                      \
        }                                                                           \
    }

    // int message_length = 0;                                                 \
    // string_t error;                                                           \
    // string_t template = get_message_template(output, format);                 \
    // message_length = fprintf(dev_null, template, expected, actual);         \
    // error = malloc(message_length);                                         \
    // sprintf(error, template, expected, actual);                             \
    // assertion_t * assertion = assertion_create(file, ln, error);              \
    // test_t * test = test_current(g_test_reporter);                                           \
    // test_add_failure(test, assertion);                                      \
    // free(template);                                                         \
    // free(error);                                                            \
    //
void print_test_result();

/* global variables to hold tests, and test results*/
test_runner_t *g_test_runner;
test_reporter_t *g_test_reporter;

/* global variables to hold module list */
function g_module_collection[MAX_MODULES];
int g_total_modules = 0;

/* default settings */
int g_setting_idle_time = 75;       /* maximum time which is to be considered ok for a test to execute */

/* BEGIN executor definition functions */
executor_definition(boolp,  string_t, "%s");
executor_definition(char,   char,   "%c");
executor_definition(short,  short,  "%d");
executor_definition(int,    int,    "%d");
executor_definition(long,   long,   "%ld");
executor_definition(float,  float,  "%f");
executor_definition(double, double, "%lf");
executor_definition(string, string_t, "%s");

void executor_bool(string_t file, line_t ln, bool_t actual, bool_t output, bool_t expected, operator_t operator)
{
    //printf("%d %d %s %s\n", actual, expected, bool_to_str(actual), bool_to_str(expected));
    executor_boolp(file, ln, bool_to_str(actual), output, bool_to_str(expected), operator);
}
/* END executor definition functions */

/* SUITE */
void pre_executor_suite(string_t description)
{
    g_test_reporter->print_description_suite(description, g_test_runner->suite_level, "");
    g_test_runner->suite_level++;
}
void post_executor_suite(string_t description)
{
    g_test_runner->suite_level--;
}

void executor_suite(string_t description, function func)
{
    pre_executor_suite(description);
    func();
    post_executor_suite(description);
}

/* TEST */
void pre_executor_test(string_t description)
{
    g_test_runner->test_collection = (test_t**)realloc(g_test_runner->test_collection,
        sizeof(test_t) * (++g_test_runner->test_count));

    g_test_runner->test_collection[g_test_runner->test_count - 1] = test_create(description, g_test_runner->suite_level);
    g_test_runner->total_success++; // assume that this test will succeed, later we will decrement this if a test failes
}

void post_executor_test(int time_taken)
{
    test_t * test = test_current(g_test_runner);
    test->time_taken = time_taken;
    if (test->is_failure)
    {
        g_test_runner->total_success--;
        g_test_runner->total_failures++;
    }
    print_test_result();
}

void executor_test(string_t description, function func)
{
    pre_executor_test(description);
    start_clock();
    func();
    stop_clock();
    post_executor_test(get_elapsed_time());
}

/* PENDING */
void pre_executor_pending(string_t description)
{
    g_test_runner->test_collection = (test_t**)realloc(g_test_runner->test_collection,
        sizeof(test_t) * (++g_test_runner->test_count));

    test_t * test = test_create(description, g_test_runner->suite_level);
    test->is_pending = true;
    g_test_runner->test_collection[g_test_runner->test_count - 1] = test;
    g_test_runner->total_pending++;
}

void post_executor_pending(string_t description)
{
    test_t * test = test_current(g_test_runner);
    g_test_reporter->print_description_pending(test, "");
}

void executor_pending(string_t description)
{
    pre_executor_pending(description);
    post_executor_pending(description);
}

/* MODULE */
void executor_register(function func)
{
    g_module_collection[g_total_modules++] = func;
}

void executor_module(function func)
{
    func();
}

void executor_all(void)
{
    int i = 0;

    for(i = 0; i < g_total_modules; i++)
    {
        g_module_collection[i]();
    }
}

void print_test_result()
{
    test_t *test = test_current(g_test_runner);
    string_t color = NULL;
    if (!test->is_failure)
    {
        g_test_reporter->print_description_success(test, "");
    }
    else
    {
        g_test_reporter->print_description_failure(test, g_test_runner->total_failures, "");
    }
}
__attribute__((constructor)) void init_test()
{
    dev_null = fopen("/dev/null", "w");
    g_test_runner = calloc(1, sizeof(test_runner_t));
    g_test_reporter = spec_reporter_init();
}

__attribute__((destructor)) void after_test()
{
    fclose(dev_null);
    g_test_reporter->print_summary(g_test_runner);
    spec_reporter_init(g_test_reporter);
}
