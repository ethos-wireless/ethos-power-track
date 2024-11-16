#include <GpuTracker.h>


GpuTracker::GpuTracker()
    : trackerID_(999){

    // Initialize NVML
    nvmlReturn_t result_ = nvmlInit();
    if (NVML_SUCCESS != result_) {
        std::cerr << "Failed to initialize NVML: " << nvmlErrorString(result_) << std::endl;
        std::abort();
    }

    // Get the number of available GPUs
    result_ = nvmlDeviceGetCount(&deviceCount_);
    CheckNvmlError(result_);

    if (deviceCount_ == 0) {
        std::cerr << "No Nvidia GPUs found." << std::endl;
        std::abort();
    }

    std::cout << "Number of GPUs: " << deviceCount_ << std::endl;

    SetGpuDeviceHandles();
    
}

GpuTracker::~GpuTracker() {
    nvmlShutdown();
}


void GpuTracker::SetGpuDeviceHandles(){
    for (uint32_t i = 0; i < deviceCount_; ++i) {
        nvmlDevice_t device;
        result_ = nvmlDeviceGetHandleByIndex(i, &device);
        CheckNvmlError(result_);
        deviceHandleVec_.push_back(device);
    }
}

std::vector<float> GpuTracker::GetTrackPowerMw(){
    std::vector<float> powerVec;
    for (auto & element : deviceHandleVec_) {
        uint32_t powerUsage;
        result_ = nvmlDeviceGetPowerUsage(element, &powerUsage);
        CheckNvmlError(result_);
        powerVec.push_back(static_cast<float>(powerUsage));
        // std::cout << "GPU " <<  << " Power Usage: " << static_cast<float>(powerUsage) << " mW" << std::endl;
    }
    return powerVec;
}


float GpuTracker::GetDeviceTrackPowerMw(uint32_t deviceId){
    // Get power usage in milliwatts
    unsigned int powerUsage;
    result_ = nvmlDeviceGetPowerUsage(deviceHandleVec_[deviceId], &powerUsage);
    CheckNvmlError(result_);
    // std::cout << "GPU " << deviceId << " Power Usage: " << static_cast<float>(powerUsage) << " mW" << std::endl;
    return static_cast<float>(powerUsage);
}


void GpuTracker::CheckNvmlError(nvmlReturn_t result) {
    if (result != NVML_SUCCESS) {
        std::cerr << "NVML error: " << nvmlErrorString(result) << std::endl;
        throw std::runtime_error("NVML error occurred");
    }
}