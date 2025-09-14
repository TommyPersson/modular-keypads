#include <Arduino.h>

#include <USB.h>
#include <SPI.h>

#include <cstdint>

#include <MacroPad/MacroPadHardware.h>
#include <MacroPad/MacroPad.h>
#include <SerialPort/SerialPort.h>
#include <Pins/OutputPin.h>
#include <Pins/InMemoryReadPort.h>
#include <RotaryEncoder/EC11RotaryEncoder.h>
#include <MCP23x17/MCP23x17.h>

const std::vector<std::uint8_t> ROW_PINS = {33, 34, 35, 36, 37, 38, 39, 40};
const std::vector<std::uint8_t> COL_PINS = {11, 12, 13, 14, 15, 16, 17, 18};

USBHIDKeyboard keyboard;
USBHIDConsumerControl consumerControl;
USBHIDSystemControl systemControl;

OutputPin indicatorLedPin(LED_BUILTIN);

std::unique_ptr<SerialPort> serial = SerialPort::from(Serial);

MacroPadHardware macroPadHardware{
    .indicatorLedPin = indicatorLedPin,
    .usb = USB,
    .serial = serial,
    .keyboard = keyboard,
    .consumerControl = consumerControl,
    .systemControl = systemControl,
};

std::unique_ptr<MacroPad> macroPad = nullptr;

std::vector<KeyBinding> keyBindings = {
    {0, 7, KeyBindingAction::consumerControl(CONSUMER_CONTROL_PLAY_PAUSE)},
    {0, 4, KeyBindingAction::consumerControl(CONSUMER_CONTROL_SCAN_PREVIOUS)},
    {0, 5, KeyBindingAction::consumerControl(CONSUMER_CONTROL_SCAN_NEXT)},
    {0, 6, KeyBindingAction::consumerControl(CONSUMER_CONTROL_MUTE)},

    {2, 0, KeyBindingAction::keyboard({HID_KEY_LANG1})},
    {2, 1, KeyBindingAction::keyboard({HID_KEY_LANG2})},
    {2, 2, KeyBindingAction::keyboard({HID_KEY_LANG3})},
    {2, 3, KeyBindingAction::keyboard({HID_KEY_LANG4})},
    {2, 7, KeyBindingAction::keyboard({HID_KEY_F24})},

    {4, 7, KeyBindingAction::consumerControl(CONSUMER_CONTROL_PAN)},

    {5, 7, KeyBindingAction::keyboard({HID_KEY_GUI_LEFT, HID_KEY_L})},


    // CONSUMER_CONTROL_PAN
    /*
    {0, 4, KeyBindingAction::keyboard({HID_KEY_CONTROL_LEFT, HID_KEY_ALT_LEFT, HID_KEY_L})},
    {0, 5, KeyBindingAction::keyboard({HID_KEY_SHIFT_LEFT, HID_KEY_F10})},
    {0, 6, KeyBindingAction::keyboard({HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_M})},
    {0, 7, KeyBindingAction::consumerControl(CONSUMER_CONTROL_CALCULATOR)},
    {1, 0, KeyBindingAction::keyboard({HID_KEY_F13})},
    {1, 1, KeyBindingAction::keyboard({HID_KEY_F14})},
    {1, 2, KeyBindingAction::keyboard({HID_KEY_F15})},
    {1, 3, KeyBindingAction::keyboard({HID_KEY_F16})},
    {1, 4, KeyBindingAction::keyboard({HID_KEY_CONTROL_LEFT, HID_KEY_KEYPAD_SUBTRACT})},
    {1, 5, KeyBindingAction::keyboard({HID_KEY_CONTROL_LEFT, HID_KEY_KEYPAD_ADD})},
    */
};

std::unique_ptr<MCP23x17> mcp23x17 = MCP23x17::SPI({
    .spiBus = HSPI,
    .pinSCK = OutputPin(1),
    .pinMOSI = OutputPin(2),
    .pinMISO = InputPin::physical(4),
    .pinCS = OutputPin(5),
});

EC11RotaryEncoder encoder1;
EC11RotaryEncoder encoder2;
EC11RotaryEncoder encoder3;
EC11RotaryEncoder encoder4;
EC11RotaryEncoder encoder5;

std::shared_ptr<KeyBindingAction> nextTab = KeyBindingAction::keyboard({HID_KEY_CONTROL_LEFT, HID_KEY_TAB});
std::shared_ptr<KeyBindingAction> previousTab = KeyBindingAction::keyboard(
    {HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_TAB}
    );

std::shared_ptr<KeyBindingAction> down = KeyBindingAction::keyboard({HID_KEY_ARROW_DOWN});
std::shared_ptr<KeyBindingAction> up = KeyBindingAction::keyboard({HID_KEY_ARROW_UP});

std::shared_ptr<KeyBindingAction> pageDown = KeyBindingAction::keyboard({HID_KEY_PAGE_DOWN});
std::shared_ptr<KeyBindingAction> pageUp = KeyBindingAction::keyboard({HID_KEY_PAGE_UP});

InMemoryReadPort portA;
InMemoryReadPort portB;

void setup() {
    USB.productName("tommy-product");
    USB.manufacturerName("tommy-manufacturer");

    mcp23x17->begin();

    macroPad = std::make_unique<MacroPad>(ROW_PINS, COL_PINS, macroPadHardware);

    macroPad->setBindings(keyBindings);
    macroPad->begin();

}

int i = 0;
int j = 0;

void handleEncoderResult(uint8_t number, EncoderDirection direction, uint8_t a, uint8_t b) {
    if (direction != EncoderDirection::NONE) {
        j++;
        if (direction == EncoderDirection::CW) {
            Serial.printf("(%x) %i direction = CW  (%x, %x)\n", number, j, a, b);
        } else if (direction == EncoderDirection::CCW) {
            Serial.printf("(%x) %i direction = CCW (%x, %x)\n", number, j, a, b);
        }
    }
}

void loop() {
    i++;
    portA.update(mcp23x17->readPortA());
    portB.update(mcp23x17->readPortB());

    // move pin construction to somewhere

    auto pinA1 = InputPin::port(0, portB);
    auto pinB1 = InputPin::port(1, portB);

    uint8_t a1 = pinA1->read();
    uint8_t b1 = pinB1->read();

    uint8_t a2 = portB.read(2);
    uint8_t b2 = portB.read(3);

    uint8_t a3 = portB.read(4);
    uint8_t b3 = portB.read(5);

    uint8_t a4 = portB.read(6);
    uint8_t b4 = portB.read(7);

    EncoderDirection direction1 = encoder1.update(a1, b1);
    EncoderDirection direction2 = encoder2.update(a2, b2);
    EncoderDirection direction3 = encoder3.update(a3, b3);
    EncoderDirection direction4 = encoder4.update(a4, b4);

    handleEncoderResult(1, direction1, a1, b1);
    handleEncoderResult(2, direction2, a2, b2);
    handleEncoderResult(3, direction3, a3, b3);
    handleEncoderResult(4, direction4, a4, b4);

    if (direction4 == EncoderDirection::CW) {
        nextTab->onKeyPressed(macroPadHardware);
        nextTab->onKeyReleased(macroPadHardware);
    } else if (direction4 == EncoderDirection::CCW) {
        previousTab->onKeyPressed(macroPadHardware);
        previousTab->onKeyReleased(macroPadHardware);
    }

    if (direction3 == EncoderDirection::CW) {
        down->onKeyPressed(macroPadHardware);
        down->onKeyReleased(macroPadHardware);
    } else if (direction3 == EncoderDirection::CCW) {
        up->onKeyPressed(macroPadHardware);
        up->onKeyReleased(macroPadHardware);
    }

    if (direction2 == EncoderDirection::CW) {
        pageDown->onKeyPressed(macroPadHardware);
        pageDown->onKeyReleased(macroPadHardware);
    } else if (direction2 == EncoderDirection::CCW) {
        pageUp->onKeyPressed(macroPadHardware);
        pageUp->onKeyReleased(macroPadHardware);
    }

    delay(1);

    macroPad->tick();
}
