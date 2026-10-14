#ifndef LIBDNF_NOMODULEMDTEST_HPP
#define LIBDNF_NOMODULEMDTEST_HPP

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "libdnf/dnf-sack.h"
#include "libdnf/hy-types.h"

class NoModulemdTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(NoModulemdTest);
        CPPUNIT_TEST(testModularMetadataIgnored);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

    void testModularMetadataIgnored();

private:
    DnfSack *sack = nullptr;
    HyRepo repo = nullptr;
    char* tmpdir = nullptr;
};


#endif //LIBDNF_NOMODULEMDTEST_HPP
