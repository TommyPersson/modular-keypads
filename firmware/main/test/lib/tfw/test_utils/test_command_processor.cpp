#include <gtest/gtest.h>
#include <cstdio>
#include <memory>
#include <string>

#include "tfw/hal/streams/OutputStream.h"
#include "tfw/utils/commands/CommandProcessor.h"

using namespace tfw::utils::commands;

namespace {
    // Collects everything written, so that responses can be checked
    class RecordingOutputStream final : public tfw::hal::streams::OutputStream {
    public:
        size_t write(const uint8_t c) override {
            text += static_cast<char>(c);
            return 1;
        }

        size_t print(const char* str) override {
            text += str;
            return std::char_traits<char>::length(str);
        }

        size_t println(const char* str) override {
            return print(str) + print("\n");
        }

        int vprintf(const char* format, va_list args) override {
            char buffer[256];
            const auto length = std::vsnprintf(buffer, sizeof(buffer), format, args);
            text += buffer;
            return length;
        }

        int printf(const char* format, ...) override {
            va_list args;
            va_start(args, format);
            const auto length = vprintf(format, args);
            va_end(args);
            return length;
        }

        void flush() override {
        }

        std::string text;
    };

    class EchoHandler final : public CommandHandler {
    public:
        EchoHandler() : CommandHandler("echo") {
        }

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override {
            executions++;
            for (const auto& arg : args) {
                responseWriter.writeLine(arg);
            }
            return tfw::utils::void_result::success();
        }

        int executions = 0;
    };

    class CommandProcessorTest : public ::testing::Test {
    protected:
        void SetUp() override {
            handler = std::make_shared<EchoHandler>();
            processor.addHandler(handler);
        }

        void receive(const std::string_view line) {
            processor.observe({.text = line});
        }

        RecordingOutputStream output;
        CommandProcessor processor{output};
        std::shared_ptr<EchoHandler> handler;
    };
}

TEST_F(CommandProcessorTest, ExecutesCommandAndRespondsWithItsId) {
    receive("7:echo:a,b");

    EXPECT_EQ(handler->executions, 1);
    EXPECT_EQ(output.text, "%7.1:a\n%7.2:b\n%7.0\n");
}

TEST_F(CommandProcessorTest, ExecutesCommandWithoutArguments) {
    receive("3:echo");

    EXPECT_EQ(handler->executions, 1);
    EXPECT_EQ(output.text, "%3.0\n");
}

// The end of a line that was cut off by a disconnect, e.g. in the middle of a firmware chunk
TEST_F(CommandProcessorTest, IgnoresLineWithoutCommandType) {
    receive("12");
    receive("QUJDRA==");

    EXPECT_EQ(handler->executions, 0);
    EXPECT_EQ(output.text, "");
}

TEST_F(CommandProcessorTest, ExecutesNextCommandAfterIgnoredLine) {
    receive("12");
    receive("8:echo:x");

    EXPECT_EQ(handler->executions, 1);
    EXPECT_EQ(output.text, "%8.1:x\n%8.0\n");
}
