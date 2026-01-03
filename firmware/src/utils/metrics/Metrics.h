#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

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

    inline std::shared_ptr<GaugeMetric> lambda_gauge(
        const std::string& name,
        const std::function<uint64_t()>& readerFn
    ) {
        return std::make_shared<LambdaGaugeMetric>(name, readerFn);
    }

    class MetricRegistry {
    public:
        void add(const std::shared_ptr<GaugeMetric>& gauge) {
            gauges.push_back(gauge);
        }

        void forEach(const std::function<void(const MetricReport&)>& callback) const {
            for (auto& gauge : gauges) {
                if (gauge != nullptr) {
                    auto report = gauge->report();
                    callback(report);
                }
            }
        }

    private:
        std::vector<std::shared_ptr<GaugeMetric>> gauges{};
    };
}
