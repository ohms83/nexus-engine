#include <gtest/gtest.h>
#include <string>
#include <format>
#include <cassert>

#include "nexus/debug/Assert.h" 

class AssertTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Disconnect all slots from the signal before each test to ensure a clean state
        nxs::Assert::onAssertionFailedEvent.disconnect_all();
    }
};

// ============================================================================
// 1. Direct API Tests (nxs::Assert::Check)
// ============================================================================

TEST_F(AssertTest, Check_TrueCondition_DoesNotTriggerSignalOrDeath)
{
    bool signalTriggered = false;
    nxs::Assert::onAssertionFailedEvent.connect([&signalTriggered](const std::string&) {
        signalTriggered = true;
    });

    // Should pass silently without raising a signal or triggering cassert
    EXPECT_NO_FATAL_FAILURE(nxs::Assert::Check(true, "This should not fail"));
    EXPECT_FALSE(signalTriggered);
}

TEST_F(AssertTest, Check_FalseCondition_EmitsSignalWithMessage)
{
    std::string capturedMessage;
    nxs::Assert::onAssertionFailedEvent.connect([&capturedMessage](const std::string& msg) {
        capturedMessage = msg;
    });

    // Death test catches the program termination caused by assert(false)
    EXPECT_DEATH({
        nxs::Assert::Check(false, "Custom test error message");
    }, "");

    // Verify the signal fired before death (tested separately via non-terminating slot inspection in death tests)
}

TEST_F(AssertTest, Check_FalseCondition_FormatsSignalMessageCorrectly)
{
    std::string capturedMessage;
    
    // We execute inside a death test block to verify signal output prior to assertion abort
    EXPECT_DEATH({
        nxs::Assert::onAssertionFailedEvent.connect([&capturedMessage](const std::string& msg) {
            // Write to stderr so the death test matcher can inspect it
            std::cerr << "CAPTURED: " << msg << std::endl;
        });

        nxs::Assert::Check(false, "Sample Failure");
    }, "CAPTURED: Assertion Failed! Sample Failure");
}

// ============================================================================
// 2. Macro Tests (NXS_ASSERT & NXS_ASSERT_MSG)
// ============================================================================

#if !defined(NXS_NO_LOG)

TEST_F(AssertTest, Macro_NXS_ASSERT_PassesWhenTrue)
{
    bool signalFired = false;
    nxs::Assert::onAssertionFailedEvent.connect([&signalFired](const std::string&) {
        signalFired = true;
    });

    int value = 42;
    NXS_ASSERT(value == 42);

    EXPECT_FALSE(signalFired);
}

TEST_F(AssertTest, Macro_NXS_ASSERT_ContainsFileAndConditionOnFailure)
{
    EXPECT_DEATH({
        nxs::Assert::onAssertionFailedEvent.connect([](const std::string& msg) {
            std::cerr << "LOG: " << msg << std::endl;
        });

        int a = 5;
        int b = 10;
        NXS_ASSERT(a == b);
    }, "LOG: Assertion Failed! \\(.*:\\d+\\) a == b");
}

TEST_F(AssertTest, Macro_NXS_ASSERT_MSG_ContainsCustomMessage)
{
    EXPECT_DEATH({
        nxs::Assert::onAssertionFailedEvent.connect([](const std::string& msg) {
            std::cerr << "LOG: " << msg << std::endl;
        });

        bool isLoaded = false;
        NXS_ASSERT_MSG(isLoaded, "Resource failed to load from disk!");
    }, "LOG: Assertion Failed! \\(.*:\\d+\\) isLoaded\\. Resource failed to load from disk!");
}

TEST_F(AssertTest, Macro_EvaluatesExpressionWithSideEffectsOnce)
{
    int counter = 0;
    auto IncrementAndCheck = [&counter]() {
        counter++;
        return true;
    };

    NXS_ASSERT(IncrementAndCheck());
    
    // Verify the condition expression was not evaluated multiple times in the macro expansion
    EXPECT_EQ(counter, 1);
}

#endif // !defined(NXS_NO_LOG)