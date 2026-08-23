#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <string_view>

#include <tfw/hal/time.h>
#include <tfw/utils/ring_queue.h>

namespace tfw::hal::metrics {
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
        explicit TimerMetric(const std::string& name, tfw::hal::time::Clock& clock) : name(name), clock(clock) {
        };
        ~TimerMetric() = default;

        void measure(const std::function<void()>& block) {
            const auto start = clock.micros();
            block();
            const auto end = clock.micros();
            const auto duration = end - start;
            timeLatest = duration;

            measurements.enqueue(duration);
        }

        void prepareReports() {
            const auto itemCount = measurements.size();
            if (itemCount == 0) {
                timeAverage = 0;
                timeMin = 0;
                timeMax = 0;
                return;
            }

            uint64_t min = std::numeric_limits<uint64_t>::max();
            uint64_t max = 0;
            uint64_t sum = 0;

            measurements.forEach([&](uint64_t time) {
                sum += time;
                min = std::min<uint64_t>(time, min);
                max = std::max<uint64_t>(time, max);
            });

            timeAverage = sum / itemCount;
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
        tfw::hal::time::Clock& clock;
        const std::string averageName = name + ".average";
        const std::string latestName = name + ".latest";
        const std::string minName = name + ".min";
        const std::string maxName = name + ".max";

        tfw::utils::ring_queue<uint64_t> measurements{100};

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
        explicit MetricRegistry(tfw::hal::time::Clock& clock) : clock(clock) {
        }

        std::shared_ptr<TimerMetric> timer(const std::string& name) {
            for (auto& timer : timers) {
                if (timer->name == name) {
                    return timer;
                }
            }

            auto timer = std::make_shared<TimerMetric>(name, clock);
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
        tfw::hal::time::Clock& clock;
        std::vector<std::shared_ptr<GaugeMetric>> gauges{};
        std::vector<std::shared_ptr<TimerMetric>> timers{};
    };
}
