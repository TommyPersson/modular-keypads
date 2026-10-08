#include "NotifierFactory.h"

using namespace mkp::components::notifications;

NotifierFactory::NotifierFactory(Print& outputStream) : outputStream(outputStream) {
}

NotifierFactory::~NotifierFactory() = default;

std::unique_ptr<Notifier> NotifierFactory::create() const {
    return std::make_unique<Notifier>(outputStream);
}
