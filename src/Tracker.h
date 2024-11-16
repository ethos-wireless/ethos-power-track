
#ifndef TRACKER_H
#define TRACKER_H

#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>

class Tracker {
public:
  Tracker();
  virtual ~Tracker();

  virtual std::string GetTrackPath() const;
  virtual void SetTrackPath();
  virtual void SetTrackPath(std::string track_path);

  virtual float GetResult() const;
  virtual void SetResult(float result);


private:
  std::string trackPath_;
  uint32_t ID_;
  float measuredResult_;
};


#endif /* TRACKER_H */