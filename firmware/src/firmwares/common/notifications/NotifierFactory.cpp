#include "NotifierFactory.h"

NotifierFactory::NotifierFactory(Print& outputStream) : outputStream(outputStream) {
}

NotifierFactory::~NotifierFactory() = default;

std::unique_ptr<Notifier> NotifierFactory::create(const uint64_t deviceId) const {
    return std::make_unique<Notifier>(deviceId, outputStream);
}
