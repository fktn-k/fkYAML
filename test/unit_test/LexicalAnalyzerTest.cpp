#include "LexicalAnalyzerTest.hpp"

#include <cstdio>

#include "fkYAML/LexicalAnalyzer.hpp"

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

    return 0;
}