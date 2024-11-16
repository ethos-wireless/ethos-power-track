
#ifndef TRACKERSCHEDULER_H
#define TRACKERSCHEDULER_H

#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>
#include <sstream>

#include <Tracker.h>
#include <CpuTracker.h>
#include <GpuTracker.h>
#include <PmdTracker.h>

struct SchedulerMeasuredResult {
    std::string dev_type;
    uint32_t call_cnt;
    float power_result;
    std::chrono::time_point<std::chrono::high_resolution_clock> measure_timestamp;
    std::chrono::duration<uint64_t,std::micro> current_interval_us;
};

class TrackerScheduler {
public:
    TrackerScheduler();
    virtual ~TrackerScheduler();

    void PrintMeasuredPower(std::chrono::duration<uint64_t,std::milli> interval);
    
    void MeasureCpuPower(std::chrono::duration<uint64_t,std::milli> interval);
    void MeasureGpuPower(std::chrono::duration<uint64_t,std::milli> interval);
    void MeasureRuPower(std::chrono::duration<uint64_t,std::milli> interval);

    std::vector<SchedulerMeasuredResult> GetAllTrackerResults() const;
    void PrintAllTrackerResults() const;
    void PeriodicalPrintResults(std::chrono::duration<uint64_t,std::milli> interval) const;

    std::string GetTimestampUs(std::chrono::time_point<std::chrono::high_resolution_clock> time_now) const;

    void Disable() {active_ = false;}
    void Enable() {active_ = true;}

private:
    // std::atomic<bool> active_;
    bool active_;

    CpuTracker cpu_tracker_;
    // std::thread cpu_thread_;
    SchedulerMeasuredResult cpu_result_;

    GpuTracker gpu_tracker_;
    // std::thread gpu_thread_;
    SchedulerMeasuredResult gpu_result_;

    PmdTracker pmd_tracker_;
    // std::thread ru_thread_;
    SchedulerMeasuredResult ru_result_;
};


#endif /* TRACKERSCHEDULER_H */