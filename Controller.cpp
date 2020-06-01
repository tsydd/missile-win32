#include "Controller.h"

#include <windows.h>

#define VK_C 0x43  // not defined in winuser.h

enum class KeyEvent {
  UNKNOWN,
  EXIT,
  LEFT_PRESSED,
  LEFT_RELEASED,
  RIGHT_PRESSED,
  RIGHT_RELEASED,
  UP_PRESSED,
  UP_RELEASED,
  DOWN_PRESSED,
  DOWN_RELEASED,
  FIRE_PRESSED,
  FIRE_RELEASED
};

enum class DeviceState {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  STOP,
  START_FIRE,
  FIRE,
  STOP_FIRE,
};

KeyEvent translateEvent(KEY_EVENT_RECORD& keyEvent) {
  switch (keyEvent.wVirtualKeyCode) {
    case VK_C:
      if (keyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
        return KeyEvent::EXIT;
      break;
    case VK_LEFT:
      return keyEvent.bKeyDown ? KeyEvent::LEFT_PRESSED
                               : KeyEvent::LEFT_RELEASED;
    case VK_RIGHT:
      return keyEvent.bKeyDown ? KeyEvent::RIGHT_PRESSED
                               : KeyEvent::RIGHT_RELEASED;
    case VK_UP:
      return keyEvent.bKeyDown ? KeyEvent::UP_PRESSED : KeyEvent::UP_RELEASED;
    case VK_DOWN:
      return keyEvent.bKeyDown ? KeyEvent::DOWN_PRESSED
                               : KeyEvent::DOWN_RELEASED;
    case VK_SPACE:
      return keyEvent.bKeyDown ? KeyEvent::FIRE_PRESSED
                               : KeyEvent::FIRE_RELEASED;
    default:
      break;
  }

  return KeyEvent::UNKNOWN;
}

DeviceState getNewDeviceState(DeviceState currentState, KeyEvent event) {
  switch (event) {
    case KeyEvent::LEFT_PRESSED:
      return DeviceState::MOVE_LEFT;
    case KeyEvent::LEFT_RELEASED:
      if (currentState == DeviceState::MOVE_LEFT)
        return DeviceState::STOP;
      break;
    case KeyEvent::RIGHT_PRESSED:
      return DeviceState::MOVE_RIGHT;
    case KeyEvent::RIGHT_RELEASED:
      if (currentState == DeviceState::MOVE_RIGHT)
        return DeviceState::STOP;
      break;
    case KeyEvent::UP_PRESSED:
      return DeviceState::MOVE_UP;
    case KeyEvent::UP_RELEASED:
      if (currentState == DeviceState::MOVE_UP) return DeviceState::STOP;
      break;
    case KeyEvent::DOWN_PRESSED:
      return DeviceState::MOVE_DOWN;
    case KeyEvent::DOWN_RELEASED:
      if (currentState == DeviceState::MOVE_DOWN)
        return DeviceState::STOP;
      break;
    case KeyEvent::FIRE_PRESSED:
      switch (currentState) {
        case DeviceState::FIRE:
          return DeviceState::STOP_FIRE;
        case DeviceState::START_FIRE:
          return currentState;
        default:
          return DeviceState::START_FIRE;
      }
    case KeyEvent::FIRE_RELEASED:
      if (currentState == DeviceState::START_FIRE)
        return DeviceState::FIRE;
      break;
  }
  return currentState;
}

void sendCommand(DeviceApi* device, DeviceState state) {
  switch (state) {
    case DeviceState::MOVE_LEFT:
      device->moveLeft();
      break;
    case DeviceState::MOVE_RIGHT:
      device->moveRight();
      break;
    case DeviceState::MOVE_UP:
      device->moveUp();
      break;
    case DeviceState::MOVE_DOWN:
      device->moveDown();
      break;
    case DeviceState::START_FIRE:
      device->fire();
      break;
    case DeviceState::STOP:
      device->stop();
      break;
    case DeviceState::STOP_FIRE:
      device->stopFire();
    default:
      break;
  }
}

void handleInput(DeviceApi* device) {
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

  if (hStdin == INVALID_HANDLE_VALUE) ExitProcess(1);

  DWORD fdwSaveOldMode;

  if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) ExitProcess(2);

  DWORD fdwMode;
  fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
  if (!SetConsoleMode(hStdin, fdwMode)) ExitProcess(3);

  bool exit = false;
  DeviceState state = DeviceState::STOP;

  while (!exit) {
    DWORD cNumRead;
    INPUT_RECORD irInBuf[128];
    if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead)) ExitProcess(4);

    for (DWORD i = 0; i < cNumRead; i++) {
      if (irInBuf[i].EventType != KEY_EVENT) continue;

      KEY_EVENT_RECORD& nativeKeyEvent = irInBuf[i].Event.KeyEvent;

      KeyEvent keyEvent = translateEvent(nativeKeyEvent);
      if (keyEvent == KeyEvent::UNKNOWN) continue;

      if (keyEvent == KeyEvent::EXIT) {
        exit = true;
        break;
      }

      DeviceState newState = getNewDeviceState(state, keyEvent);

      if (newState == state) continue;

      state = newState;
      sendCommand(device, state);
    }
  }
}