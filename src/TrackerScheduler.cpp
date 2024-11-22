#include <TrackerScheduler.h>


TrackerScheduler::TrackerScheduler()
    : active_(true){
    // cpu_tracker_ = CpuTracker();
    // gpu_tracker_ = GpuTracker();
    // pmd_tracker_ = PmdTracker(2,"","/dev/ttyUSB0",115200u);
    cpu_result_ = {"cpu_power",0,0.0,std::chrono::high_resolution_clock::now(),std::chrono::seconds(1)};
    gpu_result_ = {"gpu_power",0,0.0,std::chrono::high_resolution_clock::now(),std::chrono::seconds(1)};
    ru_result_ = {"oru_power",0,0.0,std::chrono::high_resolution_clock::now(),std::chrono::seconds(1)};
    Enable();
}

TrackerScheduler::~TrackerScheduler() {
    Disable();
}

std::string TrackerScheduler::GetTimestampUs (std::chrono::time_point<std::chrono::high_resolution_clock> time_now) const {
    using namespace std::chrono;

    auto duration_since_epoch = time_now.time_since_epoch();
    auto micros = duration_cast<microseconds>(duration_since_epoch) % 1000000;
    
    // Convert to system time
    std::time_t time_point = high_resolution_clock::to_time_t(time_now);
    std::tm tm_time = *std::localtime(&time_point);

    // Formatted timestamp with microseconds
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(6) << std::setfill('0') << micros.count(); // Display microseconds
    return oss.str();
}      

void TrackerScheduler::MeasureCpuPower(std::chrono::duration<uint64_t,std::milli> interval) {

    using namespace std::chrono;
    //Init loop condition
    uint32_t call_count = 0;
    auto call_previous_func_time = high_resolution_clock::now();
    auto next_time_point = call_previous_func_time+interval;
    float energy_previous = cpu_tracker_.GetTrackEnergyUj();
    std::this_thread::sleep_until(next_time_point);
    //Init energy measure for loop
    float energy_now = cpu_tracker_.GetTrackEnergyUj();
    float energy_diff_uj = 0;

    while (active_) {
        // cpu calc power with enerygy and actual interval
        auto call_func_time = high_resolution_clock::now();
        next_time_point += interval;  // Move to the next interval
        energy_now = cpu_tracker_.GetTrackEnergyUj();
        energy_diff_uj = energy_now - energy_previous;
        auto after_func_time = high_resolution_clock::now();
        std::chrono::microseconds actual_interval_us = 
            std::chrono::duration_cast<std::chrono::microseconds>(call_func_time-call_previous_func_time);
        uint64_t actual_interval_count_us = actual_interval_us.count();
        float cpu_power_watts = energy_diff_uj / static_cast<float>(actual_interval_count_us);
        call_previous_func_time = call_func_time;
        energy_previous = energy_now;
        // check whether exec time exceeds interval
        std::chrono::microseconds func_exec_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(after_func_time-call_func_time);
        uint64_t func_exec_duration_us_count = func_exec_duration.count();
        uint64_t interval_us_count = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(interval).count());
        if (func_exec_duration_us_count > interval_us_count) {
            std::cout << __PRETTY_FUNCTION__ << ": func_exec_duration_us_count exceed interval - func_exec_duration_us_count: "
            << func_exec_duration_us_count << " us , interval: "
            << interval_us_count << " us" << std::endl;
        }

        call_count++;
        // result format : 
        // {std::string dev_type; uint32_t call_cnt;float power_result;std::time_t measure_timestamp;std::chrono::duration<uint64_t,std::micro> current_interval;
        cpu_result_ = {"cpu_power",call_count,cpu_power_watts,call_func_time,actual_interval_us};

        // // Print results for debugging
        // std::string timestamp = GetTimestampUs(call_func_time);
        // std::cout << std::fixed << std::setprecision(6)
        //           << "Measured Type: " << cpu_result_.dev_type << " , "
        //           << "Func Timestamp: " << GetTimestampUs(cpu_result_.measure_timestamp) << " , "
        //           << "Call " << cpu_result_.call_cnt << " , " 
        //           << "Exec Time: " << cpu_result_.current_interval_us.count() << " us ,"
        //           << "CPU Power: " << cpu_result_.power_result << " W , "
        //           << std::endl;

        std::this_thread::sleep_until(next_time_point);
    }

}

void TrackerScheduler::MeasureGpuPower(std::chrono::duration<uint64_t,std::milli> interval) {
    using namespace std::chrono;
    //Init loop condition
    uint32_t call_count = 0;
    auto call_previous_func_time = high_resolution_clock::now();
    auto next_time_point = call_previous_func_time+interval;
    float power_now_mw = gpu_tracker_.GetDeviceTrackPowerMw(0);
    std::this_thread::sleep_until(next_time_point);

    while (active_) {
        // gpu directly call power and get actual interval
        auto call_func_time = high_resolution_clock::now();
        next_time_point += interval;  // Move to the next interval
        power_now_mw = gpu_tracker_.GetDeviceTrackPowerMw(0);
        float power_now_watts = (power_now_mw * static_cast<float>(1e-3));
        auto after_func_time = high_resolution_clock::now();
        std::chrono::microseconds actual_interval_us = 
            std::chrono::duration_cast<std::chrono::microseconds>(call_func_time-call_previous_func_time);
        // uint64_t actual_interval_count_us = actual_interval_us.count();
        call_previous_func_time = call_func_time;
        // check whether exec time exceeds interval
        std::chrono::microseconds func_exec_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(after_func_time-call_func_time);
        uint64_t func_exec_duration_us_count = func_exec_duration.count();
        uint64_t interval_us_count = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(interval).count());
        if (func_exec_duration_us_count > interval_us_count) {
            std::cout << __PRETTY_FUNCTION__  << ": func_exec_duration_us_count exceed interval - func_exec_duration_us_count: "
            << func_exec_duration_us_count << " us , interval: "
            << interval_us_count << " us" << std::endl;
        }

        call_count++;

        // result format : 
        // {std::string dev_type; uint32_t call_cnt;float power_result;std::time_t measure_timestamp;std::chrono::duration<uint64_t,std::micro> current_interval;
        gpu_result_ = {"gpu_power",call_count,power_now_watts,call_func_time,actual_interval_us};

        // // Print results for debugging
        // std::string timestamp = GetTimestampUs(call_func_time);
        // std::cout << std::fixed << std::setprecision(6)
        //           << "Measured Type: " << gpu_result_.dev_type << " , "
        //           << "Func Timestamp: " << GetTimestampUs(gpu_result_.measure_timestamp) << " , "
        //           << "Call Idx" << gpu_result_.call_cnt << " , " 
        //           << "Exec Time: " << gpu_result_.current_interval_us.count() << " us ,"
        //           << "GPU Power: " << gpu_result_.power_result << " W , "
        //           << std::endl;

        std::this_thread::sleep_until(next_time_point);
    }
}

void TrackerScheduler::MeasureRuPower(std::chrono::duration<uint64_t,std::milli> interval) {
    using namespace std::chrono;
    //Init loop condition
    uint32_t call_count = 0;
    auto call_previous_func_time = high_resolution_clock::now();
    auto next_time_point = call_previous_func_time+interval;
    float power_now_watts = pmd_tracker_.GetPmdPower()[0];
    std::this_thread::sleep_until(next_time_point);

    while (active_) {
        // gpu directly call power and get actual interval
        auto call_func_time = high_resolution_clock::now();
        next_time_point += interval;  // Move to the next interval
        power_now_watts = pmd_tracker_.GetPmdPower()[0];
        auto after_func_time = high_resolution_clock::now();
        std::chrono::microseconds actual_interval_us = 
            std::chrono::duration_cast<std::chrono::microseconds>(call_func_time-call_previous_func_time);
        call_previous_func_time = call_func_time;
        // check whether exec time exceeds interval
        std::chrono::microseconds func_exec_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(after_func_time-call_func_time);
        uint64_t func_exec_duration_us_count = func_exec_duration.count();
        uint64_t interval_us_count = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(interval).count());
        if (func_exec_duration_us_count > interval_us_count) {
            std::cout << __PRETTY_FUNCTION__  << ": func_exec_duration_us_count exceed interval - func_exec_duration_us_count: "
            << func_exec_duration_us_count << " us , interval: "
            << interval_us_count << " us" << std::endl;
        }

        call_count++;

        // result format : 
        // {std::string dev_type; uint32_t call_cnt;float power_result;std::time_t measure_timestamp;std::chrono::duration<uint64_t,std::micro> current_interval;
        if (power_now_watts > 0){
            ru_result_ = {"oru_power",call_count,power_now_watts,call_func_time,actual_interval_us};
        }
        std::this_thread::sleep_until(next_time_point);
    }
}

void TrackerScheduler::PrintAllTrackerResults() const {
    std::cout << std::fixed << std::setprecision(6) 
                << "-----------------------------------------------------------  " 
                << "Current Timestamp: " << GetTimestampUs(std::chrono::high_resolution_clock::now())
                << "  -----------------------------------------------------------" << std::endl;
    std::cout << std::fixed << std::setprecision(6)
                << "Measured Type: " << cpu_result_.dev_type << " , "
                << "Func Timestamp: " << GetTimestampUs(cpu_result_.measure_timestamp) << " , "
                << "CPU Power: " << std::setw(12) << cpu_result_.power_result << " W , "
                << "Interval: " << std::setw(12) << cpu_result_.current_interval_us.count() << " us ,"
                << "Call: " << std::setw(12) << cpu_result_.call_cnt << " , " 
                << std::endl;
    std::cout << std::fixed << std::setprecision(6)
                << "Measured Type: " << gpu_result_.dev_type << " , "
                << "Func Timestamp: " << GetTimestampUs(gpu_result_.measure_timestamp) << " , "
                << "GPU Power: " << std::setw(12) << gpu_result_.power_result << " W , "
                << "Interval: " << std::setw(12) << gpu_result_.current_interval_us.count() << " us ,"
                << "Call: " << std::setw(12) << gpu_result_.call_cnt << " , " 
                << std::endl; 
    std::cout << std::fixed << std::setprecision(6)
                << "Measured Type: " << ru_result_.dev_type << " , "
                << "Func Timestamp: " << GetTimestampUs(ru_result_.measure_timestamp) << " , "
                << "RU Power: " << std::setw(12) << ru_result_.power_result << " W , "
                << "Interval: " << std::setw(12) << ru_result_.current_interval_us.count() << " us ,"
                << "Call: " << std::setw(12) << ru_result_.call_cnt << " , " 
                << std::endl; 
    std::cout << "------------------------------------------------------------------------------------"
                << "-----------------------------------------------------------------------------------" << std::endl;
}

void TrackerScheduler::PeriodicalPrintResults(std::chrono::duration<uint64_t,std::milli> interval) const {
    using namespace std::chrono;
    auto next_time_point = high_resolution_clock::now();
    while (active_) {
        next_time_point += interval;  // Move to the next interval
        PrintAllTrackerResults();
        std::this_thread::sleep_until(next_time_point);
    }
}

std::vector<SchedulerMeasuredResult> TrackerScheduler::GetAllTrackerResults() const {
    std::vector<SchedulerMeasuredResult> vector_results = {cpu_result_, gpu_result_,ru_result_};
    return vector_results; 
}
