#include "LexicalAnalyzerTest.hpp"

#include <cinttypes>
#include <cstdio>

#include "fkYAML/LexicalAnalyzer.hpp"

int LexicalAnalyzerTest::ScanNullTest1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("null");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::NULL_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    void* scanned_value = lexer.GetNull();
    if (scanned_value != nullptr)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "nullptr",
            scanned_value ? "not nullptr": "nullptr");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNullTest2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("Null");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::NULL_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    void* scanned_value = lexer.GetNull();
    if (scanned_value != nullptr)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "nullptr",
            scanned_value ? "not nullptr": "nullptr");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNullTest3()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("NULL");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::NULL_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    void* scanned_value = lexer.GetNull();
    if (scanned_value != nullptr)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "nullptr",
            scanned_value ? "not nullptr": "nullptr");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNullTest4()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("~");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::NULL_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    void* scanned_value = lexer.GetNull();
    if (scanned_value != nullptr)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "nullptr",
            scanned_value ? "not nullptr": "nullptr");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanTrueBooleanTest1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("true");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::BOOLEAN_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    bool scanned_value = lexer.GetBoolean();
    if (scanned_value != true)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "true",
            scanned_value ? "true": "false");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanTrueBooleanTest2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("True");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::BOOLEAN_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    bool scanned_value = lexer.GetBoolean();
    if (scanned_value != true)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "true",
            scanned_value ? "true": "false");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanTrueBooleanTest3()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("TRUE");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::BOOLEAN_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    bool scanned_value = lexer.GetBoolean();
    if (scanned_value != true)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "true",
            scanned_value ? "true": "false");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFalseBooleanTest1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("false");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::BOOLEAN_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    bool scanned_value = lexer.GetBoolean();
    if (scanned_value != false)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "false",
            scanned_value ? "true": "false");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFalseBooleanTest2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("False");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::BOOLEAN_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    bool scanned_value = lexer.GetBoolean();
    if (scanned_value != false)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "false",
            scanned_value ? "true": "false");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFalseBooleanTest3()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("FALSE");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::BOOLEAN_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::BOOLEAN_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    bool scanned_value = lexer.GetBoolean();
    if (scanned_value != false)
    {
        std::fprintf(
            stderr,
            "Invalid scanned value. expectated=%s, actual=%s\n",
            "false",
            scanned_value ? "true": "false");
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanSignedDecimalIntegerTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("-1234");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::SIGNED_INT_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::SIGNED_INT_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    int64_t scanned_value = lexer.GetSignedInt();
    if (scanned_value != -1234)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%d, actual=%" PRId64 "\n", -1234, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanUnsignedDecimalIntegerTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("5678");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    uint64_t scanned_value = lexer.GetUnsignedInt();
    if (scanned_value != 5678ULL)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%" PRIu64 ", actual=%" PRIu64 "\n", 5678ULL, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanOctalIntegerTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("0o437");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    uint64_t scanned_value = lexer.GetUnsignedInt();
    if (scanned_value != 287ULL)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%" PRIu64 ", actual=%" PRIu64 "\n", 287ULL, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanHexadecimalIntegerTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("0xA04F");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    uint64_t scanned_value = lexer.GetUnsignedInt();
    if (scanned_value != 41039ULL)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%" PRIu64 ", actual=%" PRIu64 "\n", 41039ULL, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanSignedFloatNumberTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("-1.234");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (scanned_value != -1.234)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", -1.234, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanUnsignedFloatNumberTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("567.8");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (scanned_value != 567.8)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", 567.8, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFloatNumberWithExponentTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("9.8e-3");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (scanned_value != 9.8e-3)
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", 9.8e-3, scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanPositiveInfinityTest1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer(".inf");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isinf(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", std::numeric_limits<double>::infinity(), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanPositiveInfinityTest2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer(".Inf");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isinf(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", std::numeric_limits<double>::infinity(), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanPositiveInfinityTest3()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer(".INF");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isinf(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", std::numeric_limits<double>::infinity(), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNegativeInfinityTest1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("-.inf");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isinf(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", -std::numeric_limits<double>::infinity(), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNegativeInfinityTest2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("-.Inf");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isinf(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", -std::numeric_limits<double>::infinity(), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNegativeInfinityTest3()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("-.INF");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isinf(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", -std::numeric_limits<double>::infinity(), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNaNTest1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer(".nan");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isnan(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", std::nan(""), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNaNTest2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer(".NaN");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isnan(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", std::nan(""), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanNaNTest3()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer(".NAN");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    double scanned_value = lexer.GetFloatNumber();
    if (!std::isnan(scanned_value))
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=%lf, actual=%lf\n", std::nan(""), scanned_value);
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanDoubleQuotedEmptyStringTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("\"\"");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::STRING_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::STRING_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    std::string scanned_value = lexer.GetString();
    if (!scanned_value.empty())
    {
        std::fprintf(stderr, "Invalid scanned value. expectated=\"%s\", actual=\"%s\"\n", "", scanned_value.c_str());
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanSingleQuotedEmptyStringTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("\'\'");

    fkyaml::LexicalTokenType ret = fkyaml::LexicalTokenType::END_OF_BUFFER;
    try
    {
        ret = lexer.GetNextToken();
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    if (ret != fkyaml::LexicalTokenType::STRING_VALUE)
    {
        std::fprintf(
            stderr,
            "Invalid scan result for signed integers. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::LexicalTokenType::STRING_VALUE),
            static_cast<int>(ret));
        return -1;
    }

    std::string scanned_value = lexer.GetString();
    if (!scanned_value.empty())
    {
        std::fprintf(stderr, "Invalid scanned value. expected=\"%s\", actual=\"%s\"\n", "", scanned_value.c_str());
        return -1;
    }

    return 0;
}

int LexicalAnalyzerTest::ScanKeyBooleanValuePairTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: true");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::BOOLEAN_VALUE,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_value = "test";
    bool bool_value = true;

    for (int i = 0; i < 4; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for mapping object. expect=%d, actual=%d\n",
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_value.c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
            }

            if (ret == fkyaml::LexicalTokenType::BOOLEAN_VALUE)
            {
                if (lexer.GetBoolean() != bool_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%s, actual=%s\n",
                        bool_value ? "true" : "false",
                        lexer.GetBoolean() ? "true" : "false");
                    return -1;
                }
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanKeySignedIntegerValuePairTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: -5784");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::SIGNED_INT_VALUE,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_value = "test";
    int64_t signed_int_value = -5784;

    for (int i = 0; i < 4; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for mapping object. expect=%d, actual=%d\n",
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_value.c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
            }

            if (ret == fkyaml::LexicalTokenType::SIGNED_INT_VALUE)
            {
                if (lexer.GetSignedInt() != signed_int_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%" PRId64 ", actual=%" PRId64 "\n",
                        signed_int_value,
                        lexer.GetSignedInt());
                    return -1;
                }
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanKeyUnsignedIntegerValuePairTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: 47239");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_value = "test";
    uint64_t unsigned_int_value = 47239;

    for (int i = 0; i < 4; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for mapping object. expect=%d, actual=%d\n",
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_value.c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
            }

            if (ret == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE)
            {
                if (lexer.GetUnsignedInt() != unsigned_int_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%" PRIu64 ", actual=%" PRIu64 "\n",
                        unsigned_int_value,
                        lexer.GetUnsignedInt());
                    return -1;
                }
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanKeyFloatNumberValuePairTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: -5.58e-3");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_value = "test";
    double float_value = -5.58e-3;

    for (int i = 0; i < 4; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for mapping object. expect=%d, actual=%d\n",
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_value.c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
            }

            if (ret == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
            {
                if (lexer.GetFloatNumber() != float_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%lf, actual=%lf\n",
                        float_value,
                        lexer.GetFloatNumber());
                    return -1;
                }
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanKeyStringValuePairTest()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: some value");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string values[] =
    {
        "test",
        "some value"
    };

    for (int i = 0; i < 4; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for mapping object. expect=%d, actual=%d\n",
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != values[i / 2])
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        values[i / 2].c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFlowSequenceObject1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: [ foo, bar ]");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::SEQUENCE_FLOW_BEGIN,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::VALUE_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::SEQUENCE_FLOW_END,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_values[] =
    {
        "test",
        "foo",
        "bar",
    };
    int str_value_index = 0;

    for (int i = 0; i < 8; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for sequence object(i=%d). expect=%d, actual=%d\n",
                    i,
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_values[str_value_index])
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_values[str_value_index].c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
                ++str_value_index;
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFlowSequenceObject2()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: [ { foo: one, bar: false }, { foo: two, bar: false } ]");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::SEQUENCE_FLOW_BEGIN,
        fkyaml::LexicalTokenType::MAPPING_FLOW_BEGIN,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::VALUE_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::BOOLEAN_VALUE,
        fkyaml::LexicalTokenType::MAPPING_FLOW_END,
        fkyaml::LexicalTokenType::VALUE_SEPARATOR,
        fkyaml::LexicalTokenType::MAPPING_FLOW_BEGIN,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::VALUE_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::BOOLEAN_VALUE,
        fkyaml::LexicalTokenType::MAPPING_FLOW_END,
        fkyaml::LexicalTokenType::SEQUENCE_FLOW_END,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_values[] =
    {
        "test",
        "foo",
        "one",
        "bar",
        "foo",
        "two",
        "bar",
    };
    int str_value_index = 0;
    bool bool_value = false;

    for (int i = 0; i < 24; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for sequence object(i=%d). expect=%d, actual=%d\n",
                    i,
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_values[str_value_index])
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_values[str_value_index].c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
                ++str_value_index;
                continue;
            }

            if (ret == fkyaml::LexicalTokenType::BOOLEAN_VALUE)
            {
                if (lexer.GetBoolean() != bool_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%s, actual=%s\n",
                        bool_value ? "true" : "false",
                        lexer.GetBoolean() ? "true" : "false");
                    return -1;
                }
                continue;
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}

int LexicalAnalyzerTest::ScanFlowMappingObject1()
{
    fkyaml::LexicalAnalyzer lexer;
    lexer.SetInputBuffer("test: { bool: true, foo: bar, pi: 3.14 }");

    fkyaml::LexicalTokenType types[] =
    {
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::MAPPING_FLOW_BEGIN,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::BOOLEAN_VALUE,
        fkyaml::LexicalTokenType::VALUE_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::VALUE_SEPARATOR,
        fkyaml::LexicalTokenType::STRING_VALUE,
        fkyaml::LexicalTokenType::KEY_SEPARATOR,
        fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE,
        fkyaml::LexicalTokenType::MAPPING_FLOW_END,
        fkyaml::LexicalTokenType::END_OF_BUFFER,
    };
    std::string str_values[] =
    {
        "test",
        "bool",
        "foo",
        "bar",
        "pi",
    };
    int str_value_index = 0;
    bool bool_value = true;
    double float_value = 3.14;

    for (int i = 0; i < 16; ++i)
    {
        try
        {
            fkyaml::LexicalTokenType ret = lexer.GetNextToken();
            if (ret != types[i])
            {
                std::fprintf(
                    stderr,
                    "Invalid scan result for mapping object(i=%d). expect=%d, actual=%d\n",
                    i,
                    static_cast<int>(types[i]),
                    static_cast<int>(ret));
                return -1;
            }

            if (ret == fkyaml::LexicalTokenType::STRING_VALUE)
            {
                if (lexer.GetString() != str_values[str_value_index])
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=\"%s\", actual=\"%s\"\n",
                        str_values[str_value_index].c_str(),
                        lexer.GetString().c_str());
                    return -1;
                }
                ++str_value_index;
                continue;
            }

            if (ret == fkyaml::LexicalTokenType::BOOLEAN_VALUE)
            {
                if (lexer.GetBoolean() != bool_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%s, actual=%s\n",
                        bool_value ? "true" : "false",
                        lexer.GetBoolean() ? "true" : "false");
                    return -1;
                }
                continue;
            }

            if (ret == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE)
            {
                if (lexer.GetFloatNumber() != float_value)
                {
                    std::fprintf(
                        stderr,
                        "Invalid scanned value. expect=%lf, actual=%lf\n",
                        float_value,
                        lexer.GetFloatNumber());
                    return -1;
                }
                continue;
            }
        }
        catch (const fkyaml::Exception& e)
        {
            std::fprintf(stderr, "fkYAML internal error(i=%d): %s\n", i, e.what());
            return -1;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "unexpected error(i=%d): %s\n", i, e.what());
            return -1;
        }
    }

    return 0;
}
