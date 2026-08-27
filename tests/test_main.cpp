#include <iostream>
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

#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

int main() {

    size_t passed = 0;
    size_t failed = 0;
    auto& tests = GetTestRegistry();

    std::cout << "[RUN] Running " << tests.size() << " ForgeUI tests...\n";

    for (const auto& test : tests) {
        bool ok = false;
        try {
            ok = test.func();
        } catch (...) {
            ok = false;
        }

        if (ok) {
            std::cout << "  [PASS] " << test.name << "\n";
            ++passed;
        } else {
            std::cout << "  [FAIL] " << test.name << "\n";
            ++failed;
        }
    }

    std::cout << "\nTest Summary: " << passed << " passed, " << failed << " failed.\n";
    return (failed == 0) ? 0 : 1;
}
