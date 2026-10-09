#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "tfw/hal/streams/InputStream.h"
#include "tfw/utils/streams/LineStreamer.h"

using namespace tfw::utils::streams;

namespace {
    // Input stream whose available bytes are controlled by the test, to simulate data arriving in parts
    class FakeInputStream final : public tfw::hal::streams::InputStream {
    public:
        void deliver(const std::string& data) {
            pending += data;
        }

        int available() override {
            return static_cast<int>(pending.size() - position);
        }

        int read() override {
            if (position >= pending.size()) {
                return -1;
            }

            return static_cast<unsigned char>(pending[position++]);
        }

        int peek() override {
            if (position >= pending.size()) {
                return -1;
            }

            return static_cast<unsigned char>(pending[position]);
        }

    private:
        std::string pending;
        size_t position = 0;
    };

    // Copies the lines, as the event's text only stays valid during the notification
    class LineCollector final : public tfw::utils::observables::Observer<LineReceivedEvent> {
    public:
        std::vector<std::string> lines;

        void observe(const LineReceivedEvent& event) override {
            lines.emplace_back(event.text);
        }
    };
}

class LineStreamerTest : public ::testing::Test {
protected:
    FakeInputStream inputStream;
    LineCollector collector;

    // Delivers each part and updates the streamer in between, as if each part arrived between two main loop runs
    void deliverInParts(LineStreamer& streamer, const std::vector<std::string>& parts) {
        for (const auto& part : parts) {
            inputStream.deliver(part);
            streamer.update();
        }
    }
};

// ============================================================================
// Complete lines
// ============================================================================

TEST_F(LineStreamerTest, EmitsLineReceivedInOneRead) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"12:list.connected.devices\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"12:list.connected.devices"}));
}

TEST_F(LineStreamerTest, EmitsSeveralLinesReceivedInOneRead) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"1:ping\n2:list.connected.devices\n3:read.metrics\n"});

    EXPECT_EQ(
        collector.lines,
        std::vector<std::string>({"1:ping", "2:list.connected.devices", "3:read.metrics"})
    );
}

TEST_F(LineStreamerTest, EmitsNothingForIncompleteLine) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"12:list.connected"});

    EXPECT_TRUE(collector.lines.empty());
}

TEST_F(LineStreamerTest, IgnoresEmptyLines) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"\n1:ping\n\n2:ping\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"1:ping", "2:ping"}));
}

// ============================================================================
// Lines split across reads
// ============================================================================

TEST_F(LineStreamerTest, EmitsLineSplitAcrossTwoReads) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"12:list.conn", "ected.devices\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"12:list.connected.devices"}));
}

TEST_F(LineStreamerTest, EmitsLineSplitAcrossManyReads) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"1", "2:li", "st.conn", "ected.dev", "ices", "\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"12:list.connected.devices"}));
}

TEST_F(LineStreamerTest, EmitsLineCompletedInSameReadAsNextLineStarts) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"1:pi", "ng\n2:list.conn", "ected.devices\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"1:ping", "2:list.connected.devices"}));
}

// ============================================================================
// Carriage returns
// ============================================================================

TEST_F(LineStreamerTest, EmitsLinesEndingWithCarriageReturnAndNewline) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"1:ping\r\n2:list.connected.devices\r\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"1:ping", "2:list.connected.devices"}));
}

TEST_F(LineStreamerTest, EmitsLineWithCarriageReturnAndNewlineSplitAcrossReads) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {"1:ping\r", "\n2:ping\r\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"1:ping", "2:ping"}));
}

// ============================================================================
// Reads over 255 bytes
// ============================================================================

TEST_F(LineStreamerTest, EmitsLineLongerThan255BytesReceivedInOneRead) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    const auto line = "1:save.macro:" + std::string(300, 'a');
    deliverInParts(streamer, {line + "\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({line}));
}

TEST_F(LineStreamerTest, EmitsAllLinesWhenOneReadIsLongerThan255Bytes) {
    LineStreamer streamer(inputStream);
    streamer.addObserver(&collector);

    std::vector<std::string> expectedLines;
    std::string data;
    for (int i = 0; i < 30; i++) {
        expectedLines.push_back(std::to_string(i) + ":list.connected.devices");
        data += expectedLines.back() + "\n";
    }
    ASSERT_GT(data.size(), 255);

    deliverInParts(streamer, {data});

    EXPECT_EQ(collector.lines, expectedLines);
}

// ============================================================================
// Lines longer than the buffer
// ============================================================================

TEST_F(LineStreamerTest, DiscardsLineLongerThanBufferAndEmitsNextLine) {
    LineStreamer streamer(inputStream, 16);
    streamer.addObserver(&collector);

    deliverInParts(streamer, {std::string(40, 'a') + "\n", "1:ping\n"});

    EXPECT_EQ(collector.lines, std::vector<std::string>({"1:ping"}));
}
