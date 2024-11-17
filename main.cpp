#include <TrackingModule.h>


int main() {
    std::cout << "Starting power measurement at 1ms intervals..." << std::endl;

    TrackerScheduler track_scheduler;

    // Run the measurement function in a separate thread
    std::chrono::duration<uint64_t,std::milli> cpu_measure_interval = std::chrono::milliseconds(1);
    std::chrono::duration<uint64_t,std::milli> gpu_measure_interval = std::chrono::milliseconds(10);
    std::chrono::duration<uint64_t,std::milli> ru_measure_interval = std::chrono::milliseconds(1);
    std::chrono::duration<uint64_t,std::milli> print_interval = std::chrono::milliseconds(1);

    std::thread cpu_power_thread(&TrackerScheduler::MeasureCpuPower,&track_scheduler,cpu_measure_interval);
    std::thread gpu_power_thread(&TrackerScheduler::MeasureGpuPower,&track_scheduler,gpu_measure_interval);
    std::thread ru_power_thread(&TrackerScheduler::MeasureRuPower,&track_scheduler,ru_measure_interval);
    std::thread print_thread(&TrackerScheduler::PeriodicalPrintResults,&track_scheduler,print_interval);
    
    // Allow the measurement to run for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // Stop the measurement
    track_scheduler.Disable();
    
    // Wait for the power measurement thread to finish
    // power_thread.join();
    cpu_power_thread.join();
    gpu_power_thread.join();
    ru_power_thread.join();
    print_thread.join();

    std::cout << "Measurement completed." << std::endl;

    return 0;
}