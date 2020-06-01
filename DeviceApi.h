#pragma once
class DeviceApi {
 public:
  virtual void moveLeft() = 0;
  virtual void moveRight() = 0;
  virtual void moveUp() = 0;
  virtual void moveDown() = 0;
  virtual void stop() = 0;

  virtual void fire() = 0;
  virtual void stopFire() = 0;
};
