#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include "tfw/hal/metrics/Metrics.h"
#include "../VirtualClock.h"

using namespace tfw::hal::metrics;
using namespace tfw::hal::time::test;

class MetricsTest : public ::testing::Test {
protected:
    VirtualClock clock;
};

// ============================================================================
// GaugeMetric Tests
// ============================================================================

TEST_F(MetricsTest, GaugeMetricDefaultReport) {
    GaugeMetric gauge("test.gauge");
    auto report = gauge.report();

    EXPECT_EQ(report.name, "test.gauge");
    EXPECT_EQ(report.value, 0);
}

TEST_F(MetricsTest, GaugeMetricNamePreserved) {
    GaugeMetric gauge("my.metric.name");
    auto report = gauge.report();

    EXPECT_EQ(report.name, "my.metric.name");
}

// ============================================================================
// LambdaGaugeMetric Tests
// ============================================================================

TEST_F(MetricsTest, LambdaGaugeMetricCallsReaderFunction) {
    int counter = 0;
    auto metric = std::make_shared<LambdaGaugeMetric>(
        "counter",
        [&counter]() { return counter; }
    );

    EXPECT_EQ(metric->report().value, 0);

    counter = 42;
    EXPECT_EQ(metric->report().value, 42);

    counter = 100;
    EXPECT_EQ(metric->report().value, 100);
}

TEST_F(MetricsTest, LambdaGaugeMetricMultipleReports) {
    uint64_t state = 10;
    auto metric = std::make_shared<LambdaGaugeMetric>(
        "state",
        [&state]() { return state * 2; }
    );

    EXPECT_EQ(metric->report().value, 20);
    state = 25;
    EXPECT_EQ(metric->report().value, 50);
}

// ============================================================================
// TimerMetric Tests
// ============================================================================

TEST_F(MetricsTest, TimerMetricSingleMeasurement) {
    TimerMetric timer("op.time", clock);

    clock.setTimeUs(0);
    timer.measure([this]() {
        clock.advanceUs(100);
    });

    timer.prepareReports();

    auto latest = timer.reportLatest();
    EXPECT_EQ(latest.value, 100);
}

TEST_F(MetricsTest, TimerMetricMultipleMeasurements) {
    TimerMetric timer("op.time", clock);

    // Measure 1: 100us
    clock.setTimeUs(0);
    timer.measure([this]() {
        clock.advanceUs(100);
    });

    // Measure 2: 200us
    timer.measure([this]() {
        clock.advanceUs(200);
    });

    // Measure 3: 150us
    timer.measure([this]() {
        clock.advanceUs(150);
    });

    timer.prepareReports();

    auto latest = timer.reportLatest();
    EXPECT_EQ(latest.value, 150);
}

TEST_F(MetricsTest, TimerMetricMinMaxAverage) {
    TimerMetric timer("op.time", clock);

    // Enqueue measurements: 100, 200, 150
    clock.setTimeUs(0);
    timer.measure([this]() { clock.advanceUs(100); });
    timer.measure([this]() { clock.advanceUs(200); });
    timer.measure([this]() { clock.advanceUs(150); });

    timer.prepareReports();

    auto minReport = timer.reportMin();
    auto maxReport = timer.reportMax();
    auto avgReport = timer.reportAverage();

    EXPECT_EQ(minReport.value, 100);
    EXPECT_EQ(maxReport.value, 200);
    EXPECT_EQ(avgReport.value, 150);  // (100 + 200 + 150) / 3 = 150
}

TEST_F(MetricsTest, TimerMetricRingBufferOverwrite) {
    TimerMetric timer("op.time", clock);

    // Add 150 measurements (exceeds ring buffer capacity of 100)
    for (int i = 0; i < 150; i++) {
        clock.setTimeUs(0);
        int duration = i + 1;
        timer.measure([this, duration]() {
            clock.advanceUs(duration);
        });
    }

    timer.prepareReports();

    // Ring buffer keeps last 100 values (measurements 50-149)
    auto minReport = timer.reportMin();
    auto maxReport = timer.reportMax();

    EXPECT_EQ(minReport.value, 51);
    EXPECT_EQ(maxReport.value, 150);
}

TEST_F(MetricsTest, TimerMetricReportNameIncludesMetricName) {
    TimerMetric timer("api.call", clock);

    clock.setTimeUs(0);
    timer.measure([this]() { clock.advanceUs(100); });
    timer.prepareReports();

    auto latest = timer.reportLatest();
    auto average = timer.reportAverage();
    auto min = timer.reportMin();
    auto max = timer.reportMax();

    EXPECT_EQ(latest.name, "api.call.latest");
    EXPECT_EQ(average.name, "api.call.average");
    EXPECT_EQ(min.name, "api.call.min");
    EXPECT_EQ(max.name, "api.call.max");
}

TEST_F(MetricsTest, TimerMetricEmptyBeforePrepare) {
    TimerMetric timer("op.time", clock);

    timer.prepareReports();

    // All metrics should be 0 when empty
    EXPECT_EQ(timer.reportLatest().value, 0);
    EXPECT_EQ(timer.reportAverage().value, 0);
    EXPECT_EQ(timer.reportMin().value, 0);
    EXPECT_EQ(timer.reportMax().value, 0);
}

TEST_F(MetricsTest, TimerMetricConsecutiveMeasurements) {
    TimerMetric timer("op.time", clock);

    clock.setTimeUs(0);
    for (int i = 0; i < 5; i++) {
        timer.measure([this]() { clock.advanceUs(50); });
    }

    timer.prepareReports();

    auto minReport = timer.reportMin();
    auto maxReport = timer.reportMax();
    auto avgReport = timer.reportAverage();

    EXPECT_EQ(minReport.value, 50);
    EXPECT_EQ(maxReport.value, 50);
    EXPECT_EQ(avgReport.value, 50);
}

// ============================================================================
// MetricRegistry Tests
// ============================================================================

TEST_F(MetricsTest, MetricRegistryCreatesTimers) {
    MetricRegistry registry(clock);

    auto timer1 = registry.timer("op1");
    auto timer2 = registry.timer("op2");

    ASSERT_NE(timer1, nullptr);
    ASSERT_NE(timer2, nullptr);
    EXPECT_NE(timer1, timer2);
}

TEST_F(MetricsTest, MetricRegistryReusesExistingTimers) {
    MetricRegistry registry(clock);

    auto timer1 = registry.timer("op");
    auto timer2 = registry.timer("op");

    EXPECT_EQ(timer1, timer2);
}

TEST_F(MetricsTest, MetricRegistryAddGauges) {
    MetricRegistry registry(clock);

    auto gauge1 = std::make_shared<GaugeMetric>("gauge1");
    auto gauge2 = std::make_shared<GaugeMetric>("gauge2");

    registry.add(gauge1);
    registry.add(gauge2);

    // Should not throw
}

TEST_F(MetricsTest, MetricRegistryIteratesAllMetrics) {
    MetricRegistry registry(clock);

    // Add gauges
    auto gauge1 = std::make_shared<GaugeMetric>("gauge.one");
    auto gauge2 = std::make_shared<GaugeMetric>("gauge.two");
    registry.add(gauge1);
    registry.add(gauge2);

    // Add timer
    auto timer = registry.timer("timer.one");
    clock.setTimeUs(0);
    timer->measure([this]() { clock.advanceUs(100); });

    // Collect all reports
    std::vector<MetricReport> reports;
    registry.forEach([&reports](const MetricReport& report) {
        reports.push_back(report);
    });

    // Should have: 2 gauges + 4 timer metrics (average, latest, min, max)
    EXPECT_EQ(reports.size(), 6);

    // Verify gauge names are present
    bool hasGauge1 = false, hasGauge2 = false;
    for (const auto& report : reports) {
        if (report.name == "gauge.one") hasGauge1 = true;
        if (report.name == "gauge.two") hasGauge2 = true;
    }
    EXPECT_TRUE(hasGauge1);
    EXPECT_TRUE(hasGauge2);
}

TEST_F(MetricsTest, MetricRegistryTimerCallsPrepareReports) {
    MetricRegistry registry(clock);

    auto timer = registry.timer("op");

    // Measure three different durations
    clock.setTimeUs(0);
    timer->measure([this]() { clock.advanceUs(50); });
    timer->measure([this]() { clock.advanceUs(100); });
    timer->measure([this]() { clock.advanceUs(75); });

    // Collect reports - this should call prepareReports internally
    std::vector<MetricReport> reports;
    registry.forEach([&reports](const MetricReport& report) {
        reports.push_back(report);
    });

    // Find the average report
    MetricReport* avgReport = nullptr;
    for (auto& report : reports) {
        if (report.name == "op.average") {
            avgReport = &report;
            break;
        }
    }

    ASSERT_NE(avgReport, nullptr);
    EXPECT_EQ(avgReport->value, 75);  // (50 + 100 + 75) / 3 = 75
}

TEST_F(MetricsTest, MetricRegistryMultipleTimers) {
    MetricRegistry registry(clock);

    auto timer1 = registry.timer("read");
    auto timer2 = registry.timer("write");

    clock.setTimeUs(0);
    timer1->measure([this]() { clock.advanceUs(100); });
    timer2->measure([this]() { clock.advanceUs(200); });

    std::vector<MetricReport> reports;
    registry.forEach([&reports](const MetricReport& report) {
        reports.push_back(report);
    });

    // Should have 4 read metrics + 4 write metrics = 8 total
    EXPECT_EQ(reports.size(), 8);

    // Verify read metrics
    bool hasReadLatest = false, hasReadAvg = false;
    bool hasWriteLatest = false, hasWriteAvg = false;

    for (const auto& report : reports) {
        if (report.name == "read.latest") {
            hasReadLatest = true;
            EXPECT_EQ(report.value, 100);
        }
        if (report.name == "read.average") {
            hasReadAvg = true;
            EXPECT_EQ(report.value, 100);
        }
        if (report.name == "write.latest") {
            hasWriteLatest = true;
            EXPECT_EQ(report.value, 200);
        }
        if (report.name == "write.average") {
            hasWriteAvg = true;
            EXPECT_EQ(report.value, 200);
        }
    }

    EXPECT_TRUE(hasReadLatest);
    EXPECT_TRUE(hasReadAvg);
    EXPECT_TRUE(hasWriteLatest);
    EXPECT_TRUE(hasWriteAvg);
}

TEST_F(MetricsTest, MetricRegistryEmptyRegistry) {
    MetricRegistry registry(clock);

    std::vector<MetricReport> reports;
    registry.forEach([&reports](const MetricReport& report) {
        reports.push_back(report);
    });

    // Empty registry should produce no reports
    EXPECT_EQ(reports.size(), 0);
}

TEST_F(MetricsTest, MetricRegistryMixedMetrics) {
    MetricRegistry registry(clock);

    // Add multiple gauges
    int counter = 0;
    auto gauge = std::make_shared<LambdaGaugeMetric>(
        "system.count",
        [&counter]() { return counter; }
    );
    registry.add(gauge);

    // Add timers
    auto timer1 = registry.timer("request.time");
    auto timer2 = registry.timer("db.query.time");

    // Measure
    counter = 42;
    clock.setTimeUs(0);
    timer1->measure([this]() { clock.advanceUs(150); });
    timer1->measure([this]() { clock.advanceUs(250); });
    timer2->measure([this]() { clock.advanceUs(1000); });

    // Collect all reports
    std::vector<MetricReport> reports;
    registry.forEach([&reports](const MetricReport& report) {
        reports.push_back(report);
    });

    // Should have: 1 gauge + 4 request.time metrics + 4 db.query.time metrics = 9
    EXPECT_EQ(reports.size(), 9);

    // Verify gauge value
    bool hasCorrectGauge = false;
    for (const auto& report : reports) {
        if (report.name == "system.count") {
            hasCorrectGauge = true;
            EXPECT_EQ(report.value, 42);
        }
    }
    EXPECT_TRUE(hasCorrectGauge);
}

TEST_F(MetricsTest, TimerMetricIntegrationWithRegistry) {
    MetricRegistry registry(clock);

    auto timer = registry.timer("computation");

    // Simulate some variable-time operations
    clock.setTimeUs(0);
    for (int i = 0; i < 10; i++) {
        int duration = 100 + i * 10;
        timer->measure([this, duration]() {
            // Simulate operation that takes duration microseconds
            clock.advanceUs(duration);
        });
    }

    // Collect reports
    std::vector<MetricReport> reports;
    registry.forEach([&reports](const MetricReport& report) {
        reports.push_back(report);
    });

    // Find min, max, average
    uint64_t minVal = 0, maxVal = 0, avgVal = 0;
    for (const auto& report : reports) {
        if (report.name == "computation.min") minVal = report.value;
        if (report.name == "computation.max") maxVal = report.value;
        if (report.name == "computation.average") avgVal = report.value;
    }

    // Times: 100, 110, 120, 130, 140, 150, 160, 170, 180, 190
    EXPECT_EQ(minVal, 100);
    EXPECT_EQ(maxVal, 190);
    EXPECT_EQ(avgVal, 145);  // sum=1450, 1450/10=145
}
