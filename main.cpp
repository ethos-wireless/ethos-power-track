#include <TrackingModule.h>


int main(int argc, char* argv[]) {
    
    uint32_t time_seconds = 10;
    std::vector<std::string> args(argv + 1, argv + argc);
    for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++) {
        if (*it == "-h" || *it == "--help") {
            std::cout << "Usage: sudo ./ETHOS-power [options]\n";
            std::cout << "Options:\n";
            std::cout << "  -h, --help  Display this help message\n";
            std::cout << "  -t, --time  Total measurement time duration (seconds)\n";
            return 0;
        } else if (*it == "-t" || *it == "--time") {
            try {
                it = it+1;
                time_seconds = std::stoul(*it);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << std::endl;
            }
        }
        else {
            std::cout << "Unknown option: " << *it << "\n";
            return 1;
        }
    }

    std::cout << "Starting ETHOS-power measurement ..." << std::endl;
    std::cout << "Scheduled time duration: " << time_seconds << " seconds" << std::endl;

    TrackerScheduler track_scheduler;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::chrono::duration<uint64_t,std::milli> cpu_measure_interval = std::chrono::milliseconds(1);
    std::chrono::duration<uint64_t,std::milli> gpu_measure_interval = std::chrono::milliseconds(10);
    std::chrono::duration<uint64_t,std::milli> ru_measure_interval = std::chrono::milliseconds(10);
    std::chrono::duration<uint64_t,std::milli> print_interval = std::chrono::milliseconds(1);

    std::thread cpu_power_thread(&TrackerScheduler::MeasureCpuPower,&track_scheduler,cpu_measure_interval);
    std::thread gpu_power_thread(&TrackerScheduler::MeasureGpuPower,&track_scheduler,gpu_measure_interval);
    std::thread ru_power_thread(&TrackerScheduler::MeasureRuPower,&track_scheduler,ru_measure_interval);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread print_thread(&TrackerScheduler::PeriodicalPrintResults,&track_scheduler,print_interval);

    std::this_thread::sleep_for(std::chrono::seconds(time_seconds));
    
    track_scheduler.Disable();

    // power_thread.join();
    cpu_power_thread.join();
    gpu_power_thread.join();
    ru_power_thread.join();
    print_thread.join();

    std::cout << "Measurement completed." << std::endl;

    return 0;
}