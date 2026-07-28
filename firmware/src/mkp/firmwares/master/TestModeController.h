#pragma once

class TestModeController {
public:
    TestModeController() : _isEnabled(false) {}
    ~TestModeController() = default;

    void enable() { _isEnabled = true; }
    void disable() { _isEnabled = false; }
    bool isEnabled() const { return _isEnabled; }

private:
    bool _isEnabled;
};
