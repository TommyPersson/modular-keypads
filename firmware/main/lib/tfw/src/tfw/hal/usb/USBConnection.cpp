#include "USBConnection.h"

#include "USB.h"

#include "NoOpUSBConnection.h"

#ifdef SOC_USB_OTG_SUPPORTED
#include "RealUSBConnection.h"
#endif

using namespace tfw::hal::usb;

std::unique_ptr<Connection> Connection::create(time::Clock& clock) {
    #ifdef SOC_USB_OTG_SUPPORTED
        return std::make_unique<RealConnection>(clock);
    #else
        return std::make_unique<NoOpConnection>();
    #endif
}
