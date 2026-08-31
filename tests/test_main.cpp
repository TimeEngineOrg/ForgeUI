#include <cstdio>
#include <vector>
#include <string>
#include <functional>

struct TestCase {
    std::string name;
    std::function<bool()> func;
};

static std::vector<TestCase>& GetTestRegistry() {
    static std::vector<TestCase> registry;
    return registry;
}

void RegisterTest(const std::string& name, std::function<bool()> func) {
    GetTestRegistry().push_back({name, func});
}

extern void RegisterHashTests();
extern void RegisterArenaTests();
extern void RegisterStorageTests();
extern void RegisterDirtyTests();
extern void RegisterLayoutTests();
extern void RegisterDrawListTests();
extern void RegisterFontTests();
extern void RegisterWidgetsTests();
extern void RegisterJobSystemTests();
extern void RegisterBackendTests();

int main() {
    GetTestRegistry().clear();
    RegisterHashTests();
    RegisterArenaTests();
    RegisterStorageTests();
    RegisterDirtyTests();
    RegisterLayoutTests();
    RegisterDrawListTests();
    RegisterFontTests();
    RegisterWidgetsTests();
    RegisterJobSystemTests();
    RegisterBackendTests();

    size_t passed = 0;
    size_t failed = 0;
    auto& tests = GetTestRegistry();

    std::printf("====================================================\n");
    std::printf("            ForgeUI Core Test Suite                 \n");
    std::printf("====================================================\n");
    std::printf("[RUN] Running %zu ForgeUI unit tests...\n", tests.size());
    std::fflush(stdout);

    for (const auto& test : tests) {
        bool ok = false;
        try {
            ok = test.func();
        } catch (...) {
            ok = false;
        }

        if (ok) {
            std::printf("  [PASS] %s\n", test.name.c_str());
            ++passed;
        } else {
            std::printf("  [FAIL] %s\n", test.name.c_str());
            ++failed;
        }
        std::fflush(stdout);
    }

    std::printf("----------------------------------------------------\n");
    std::printf("Test Summary: %zu passed, %zu failed.\n", passed, failed);
    std::printf("====================================================\n");
    std::fflush(stdout);
    return (failed == 0) ? 0 : 1;
}
