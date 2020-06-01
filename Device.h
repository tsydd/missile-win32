#pragma once

#include <windows.h>

#include "DeviceApi.h"

class Device : public DeviceApi {
 public:
  Device();
  ~Device();

  void moveLeft() override;
  void moveRight() override;
  void moveUp() override;
  void moveDown() override;
  void stop() override;

  void fire() override;
  void stopFire() override;

 private:
  void sendCommand(const unsigned char* cmd);

  HANDLE handle;
};
