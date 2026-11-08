#include "nexus/debug/Assert.h"

#include <cstdlib>

using namespace nxs;

sigslot::signal<const std::string&> Assert::onAssertionFailedEvent;

void Assert::Check(bool condition, const std::string& message)
{
    if (!condition)
    {
        const auto formatted = std::format("Assertion Failed! {}", message);
        onAssertionFailedEvent(formatted);
        assert(condition);
        std::abort();
    }
}
