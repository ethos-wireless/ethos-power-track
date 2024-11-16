
#ifndef GPUTRACKER_H
#define GPUTRACKER_H


#include <iostream>
#include <nvml.h>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <vector>

#include <Tracker.h>

class GpuTracker : public Tracker {
public:
  GpuTracker();
  virtual ~GpuTracker() override;

  std::vector<float> GetTrackPowerMw();
  float GetDeviceTrackPowerMw(uint32_t deviceIdx);
  void CheckNvmlError(nvmlReturn_t result);
  void SetGpuDeviceHandles();


private:
  uint32_t trackerID_;
  uint32_t deviceCount_;
  nvmlReturn_t result_;
  std::vector<nvmlDevice_t> deviceHandleVec_;

};


#endif /* GPUTRACKER_H */