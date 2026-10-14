#include "NoModulemdTest.hpp"

#include "libdnf/dnf-package.h"
#include "libdnf/dnf-sack-private.hpp"
#include "libdnf/hy-iutil-private.hpp"
#include "libdnf/sack/query.hpp"

#include <algorithm>
#include <string>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(NoModulemdTest);

#define UNITTEST_DIR "/tmp/libdnfXXXXXX"

void NoModulemdTest::setUp()
{
    g_autoptr(GError) error = nullptr;

    tmpdir = g_strdup(UNITTEST_DIR);
    char *retptr = mkdtemp(tmpdir);
    CPPUNIT_ASSERT(retptr);

    sack = dnf_sack_new();
    dnf_sack_set_cachedir(sack, tmpdir);
    dnf_sack_set_arch(sack, "x86_64", NULL);
    dnf_sack_setup(sack, 0, NULL);
    // Both RPMs in this repo are modular and none of their modules is enabled,
    // so with libmodulemd they would be filtered out.
    repo = hy_repo_create("test_advisory_repo");
    std::string repodata = std::string(TESTDATADIR "/advisories/repodata/");
    hy_repo_set_string(repo, HY_REPO_MD_FN, (repodata + "repomd.xml").c_str());
    hy_repo_set_string(repo, HY_REPO_PRIMARY_FN, (repodata + "primary.xml.gz").c_str());
    hy_repo_set_string(repo, MODULES_FN, (repodata + "modules.yaml.gz").c_str());
    CPPUNIT_ASSERT(dnf_sack_load_repo(sack, repo, 0, &error));
}

void NoModulemdTest::tearDown()
{
    dnf_remove_recursive_v2(tmpdir, NULL);
    delete repo;
    g_object_unref(sack);
    g_free(tmpdir);
}

void NoModulemdTest::testModularMetadataIgnored()
{
    dnf_sack_filter_modules_v2(sack, nullptr, nullptr, tmpdir, "platform:f33", false, false, false);

    CPPUNIT_ASSERT(dnf_sack_get_module_container(sack)->empty());
    libdnf::Query query(sack);
    query.available();
    std::vector<std::string> nevras;
    for (size_t i = 0; i < query.size(); i++) {
        g_autoptr(DnfPackage) pkg = dnf_package_new(sack, query.getIndexItem(i));
        nevras.emplace_back(dnf_package_get_nevra(pkg));
    }
    std::sort(nevras.begin(), nevras.end());
    std::vector<std::string> expected{
        "test-perl-DBI-1-2.module_el8+6587+9879afr5.x86_64",
        "test-perl-DBI-1-2.module_el8+6745+9879ate3.x86_64",
    };
    CPPUNIT_ASSERT(nevras == expected);
}
