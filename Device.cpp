#include <iostream>

#include "Device.h"

using namespace std;

static const LPCWCHAR DEVICE_PATH =
    L"\\\\?\\HID#VID_0A81&PID_0701#6&19614bfc&0&0000#{4d1e55b2-f16f-11cf-88cb-"
    L"001111000030}";
static const unsigned char CMD_MOVE_DOWN[2] = {0, 0x01};
static const unsigned char CMD_MOVE_UP[2] = {0, 0x02};
static const unsigned char CMD_MOVE_LEFT[2] = {0, 0x04};
static const unsigned char CMD_MOVE_RIGHT[2] = {0, 0x08};
static const unsigned char CMD_FIRE[2] = {0, 0x10};
static const unsigned char CMD_STOP[2] = {0, 0x20};
static const unsigned char CMD_STOP_FIRE[2] = {0, 0x40};

Device::Device() {
  handle = CreateFile(DEVICE_PATH, GENERIC_READ | GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                      OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);

  if (handle == INVALID_HANDLE_VALUE) {
    auto lastResult = GetLastError();
    cout << "Failed to find the device: " << lastResult << endl;
  } else {
    cout << "Device Found" << endl;
  }
}

Device::~Device() {
  CloseHandle(handle);
  cout << "Device disconnected" << endl;
}

void Device::moveLeft() { sendCommand(CMD_MOVE_LEFT); }

void Device::moveRight() { sendCommand(CMD_MOVE_RIGHT); }

void Device::moveUp() { sendCommand(CMD_MOVE_UP); }

void Device::moveDown() { sendCommand(CMD_MOVE_DOWN); }

void Device::stop() { sendCommand(CMD_STOP); }

void Device::fire() { sendCommand(CMD_FIRE); }

void Device::stopFire() {
  // sendCommand(CMD_STOP_FIRE);
  // workaround as stopFire doesn't work
  sendCommand(CMD_MOVE_DOWN);
  sendCommand(CMD_STOP);
}

void Device::sendCommand(const unsigned char* cmd) {
  BOOL res;

  OVERLAPPED ol;
  memset(&ol, 0, sizeof(ol));
  res = WriteFile(handle, cmd, 2, nullptr, &ol);
  if (!res) {
    auto error = GetLastError();
    if (error != ERROR_IO_PENDING)
      cout << "Failed to send command to device: " << error << endl;
  }

  DWORD bytes_written;
  res = GetOverlappedResult(handle, &ol, &bytes_written, TRUE);
  if (!res) {
    auto error = GetLastError();
    // std::cout << "GetOverlappedResult: " << error << std::endl;
  }
}
