#pragma once

#include <memory>
#include <vector>

#include "MacroPadHardware.h"

class KeyBindingAction {
public:
    virtual ~KeyBindingAction() = default;

    static std::shared_ptr<KeyBindingAction> consumerControl(int usageId);
    static std::shared_ptr<KeyBindingAction> systemControl(int usageId);
    static std::shared_ptr<KeyBindingAction> keyboard(const std::vector<int>& hidKeyCodes);

    virtual void onKeyPressed(const MacroPadHardware& hardware) = 0;
    virtual void onKeyReleased(const MacroPadHardware& hardware) = 0;
};

class ConsumerControlKeyBindingAction final : public KeyBindingAction {
public:
    explicit ConsumerControlKeyBindingAction(int usageId);

    void onKeyPressed(const MacroPadHardware& hardware) override;
    void onKeyReleased(const MacroPadHardware& hardware) override;

private:
    const int usageId;
};

class SystemControlKeyBindingAction final : public KeyBindingAction {
public:
    explicit SystemControlKeyBindingAction(int usageId);

    void onKeyPressed(const MacroPadHardware& hardware) override;
    void onKeyReleased(const MacroPadHardware& hardware) override;

private:
    const int usageId;
};

class KeyboardKeyBindingAction final : public KeyBindingAction {
public:
    explicit KeyboardKeyBindingAction(const std::vector<int>& hidKeyCodes);

    void onKeyPressed(const MacroPadHardware& hardware) override;
    void onKeyReleased(const MacroPadHardware& hardware) override;

private:
    const std::vector<int> hidKeyCodes;
};
