
#ifndef CPUTRACKER_H
#define CPUTRACKER_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <ctime>
#include <sstream>

#include <Tracker.h>




class CpuTracker : public Tracker {
public:
  CpuTracker();
  CpuTracker(uint32_t ID, std::string path);
  virtual ~CpuTracker() override;


  float GetTrackEnergyUj();
  void SetTrackPath() override;
  void SetTrackPath(std::string path);

private:
  std::string trackPath_;
  uint32_t ID_;

};


#endif /* CPUTRACKER_H */