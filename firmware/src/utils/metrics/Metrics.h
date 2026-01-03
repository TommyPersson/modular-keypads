#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

#include "utils/time/Time.h"

namespace utils::metrics {
    struct MetricReport {
        std::string_view name;
        std::uint64_t value;
    };

    class GaugeMetric {
    public:
        explicit GaugeMetric(const std::string& name) : name(name) {
        }

        virtual ~GaugeMetric() = default;

        virtual MetricReport report() {
            return MetricReport{.name = name, .value = 0};
        }

    protected:
        std::string name;
    };

    class LambdaGaugeMetric final : public GaugeMetric {
    public:
        LambdaGaugeMetric(
            const std::string& name,
            const std::function<uint64_t()>& readerFn
        ) : GaugeMetric(name), readerFn(readerFn) {
        }

        ~LambdaGaugeMetric() override = default;

        MetricReport report() override {
            return {.name = name, .value = readerFn()};
        }

    private:
        std::function<uint64_t()> readerFn;
    };

    class TimerMetric {
    public:
        explicit TimerMetric(const std::string& name) : name(name) {
        };
        ~TimerMetric() = default;

        void measure(const std::function<void()>& block) {
            const auto start = time::micros();
            block();
            const auto end = time::micros();
            const auto duration = end - start;
            timeLatest = duration;

            ringData[ringNext++] = duration;

            if (ringNext >= ringMax) {
                ringNext = 0;
            }

            ringCount = std::min<uint8_t>(1 + ringCount, ringMax);
        }

        void prepareReports() {
            uint64_t min = std::numeric_limits<uint64_t>::max();
            uint64_t max = 0;
            uint64_t sum = 0;
            for (const auto time : ringData) {
                sum += time;
                min = std::min<uint64_t>(time, min);
                max = std::max<uint64_t>(time, max);
            }

            timeAverage = sum / ringCount;
            timeMin = min;
            timeMax = max;
        }

        MetricReport reportAverage() const {
            return MetricReport{.name = averageName, .value = timeAverage};
        }

        MetricReport reportLatest() const {
            return MetricReport{.name = latestName, .value = timeLatest};
        }

        MetricReport reportMin() const {
            return MetricReport{.name = minName, .value = timeMin};
        }

        MetricReport reportMax() const {
            return MetricReport{.name = maxName, .value = timeMax};
        }

        const std::string name;

    private:
        const std::string averageName = name + ".average";
        const std::string latestName = name + ".latest";
        const std::string minName = name + ".min";
        const std::string maxName = name + ".max";

        uint8_t ringNext = 0;
        uint8_t ringCount = 0;
        uint8_t ringMax = 100;
        uint64_t ringData[100]{};

        uint64_t timeAverage = 0;
        uint64_t timeLatest = 0;
        uint64_t timeMin = 0;
        uint64_t timeMax = 0;
    };

    inline std::shared_ptr<GaugeMetric> lambda_gauge(
        const std::string& name,
        const std::function<uint64_t()>& readerFn
    ) {
        return std::make_shared<LambdaGaugeMetric>(name, readerFn);
    }

    class MetricRegistry {
    public:
        std::shared_ptr<TimerMetric> timer(const std::string& name) {
            for (auto& timer : timers) {
                if (timer->name == name) {
                    return timer;
                }
            }

            auto timer = std::make_shared<TimerMetric>(name);
            timers.push_back(timer);

            return timer;
        }

        void add(const std::shared_ptr<GaugeMetric>& gauge) {
            gauges.push_back(gauge);
        }

        void forEach(const std::function<void(const MetricReport&)>& callback) const {
            for (auto& gauge : gauges) {
                callback(gauge->report());
            }

            for (auto& timer : timers) {
                timer->prepareReports();
                callback(timer->reportAverage());
                callback(timer->reportLatest());
                callback(timer->reportMin());
                callback(timer->reportMax());
            }
        }

    private:
        std::vector<std::shared_ptr<GaugeMetric>> gauges{};
        std::vector<std::shared_ptr<TimerMetric>> timers{};
    };
}
