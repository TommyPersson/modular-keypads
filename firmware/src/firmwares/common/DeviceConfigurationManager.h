#pragma once
#include <Preferences.h>
#include <WString.h>

#include "Logger.h"

class DeviceConfigurationManager {
public:
	explicit DeviceConfigurationManager(Preferences& preferences, Logger& logger);
	~DeviceConfigurationManager() = default;

	void begin() const;

	String getDeviceId() const;
	String getDeviceVersion() const;

	uint8_t getDeviceAddress() const;
	bool setDeviceAddress(uint8_t deviceAddress) const;

	char getDeviceType() const;
	bool setDeviceType(char deviceType) const;
private:
	Preferences& preferences;
	Logger& logger;
};
