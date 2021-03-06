#include "assert.h"

#include <stdio.h>

module(byte)
{
    suite("Checking if byte assertions are working fine")
    {
        suite("bool")
        {
            test("Should pass if true == true")
            {
                expect_bool(true) to be equal(true);
            } end
            test("Should pass if true != false")
            {
                expect_bool(true) to not be equal(false);
            } end
        } end
        suite("char")
        {
            test("Should pass if 'c' == 'c'")
            {
                expect_char('c') to be equal('c');
            } end
            test("Should pass if 'c' != 'd'")
            {
                expect_char('c') to not be equal('d');
            } end
        }end
    } end
}
module(number)
{
    suite("Checking if number assertions are working fine")
    {
        suite("short")
        {
            test("Should pass if 5 == 5")
            {
                expect_short(5) to be equal(5);
            } end
            test("Should pass if 5 != 6")
            {
                expect_short(5) to not be equal(6);
            } end
        } end

        suite("int")
        {
            test("Should pass if 10 == 10")
            {
                expect_int(10) to be equal(10);
            } end
            test("Should pass if 10 != 11")
            {
                expect_int(10) to not be equal(11);
            } end
        } end

        suite("long")
        {
            test("Should pass if 100L == 100L")
            {
                expect_long(100L) to be equal(100L);
            } end
            test("Should pass if 100L != 101L")
            {
                expect_long(100L) to not be equal(101L);
            } end
        } end

        suite("float")
        {
            test("Should pass if 10.02 == 10.02")
            {
                expect_float(10.02) to be equal(10.02);
            } end
            test("Should pass if 10.02 != 10.03")
            {
                expect_float(10.02) to not be equal(10.03);
            } end
        } end

        suite("double")
        {
            test("Should pass if 10.25 == 10.25")
            {
                expect_double(10.25) to be equal(10.25);
            } end
            test("Should pass if 10.25 != 10.20")
            {
                expect_double(10.25) to not be equal(10.20);
            } end
        } end
    } end
}

module(string)
{
    suite("Checking if string_t assertions are working fine")
    {
        suite("string_t (char *)")
        {
            test("Should pass if \"Hello\" == \"Hello\"")
            {
                expect_string("Hello") to be equal("Hello");
            } end
            test("Should pass if \"Hello\" != \"World\"")
            {
                expect_string("Hello") to not be equal("World");
            } end
            pending("test_t case to compare pointers");
        } end
    } end
}
