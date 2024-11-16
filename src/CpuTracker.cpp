#include <CpuTracker.h>


CpuTracker::CpuTracker()
    : ID_(999),
	  trackPath_("/sys/class/powercap/intel-rapl:0/energy_uj"){

    float energy_init = GetTrackEnergyUj();
    if (energy_init < 0) {
        std::cerr << "Error reading energy file: " << energy_init << " < 0" << std::endl;
        std::abort();
    }
}

CpuTracker::CpuTracker(uint32_t ID, std::string path)
    : ID_(ID),
	  trackPath_(path) {
}

CpuTracker::~CpuTracker() {
    
}

void CpuTracker::SetTrackPath(){
    trackPath_ = "/sys/class/powercap/intel-rapl:0/energy_uj";
}
void CpuTracker::SetTrackPath(std::string path){
    trackPath_ = path;
}

// Get Intel CPU socket energy in microjoules
float CpuTracker::GetTrackEnergyUj(){
    std::ifstream energy_file(trackPath_);
    if (!energy_file.is_open()) {
        std::cerr << "Error opening energy file: " << trackPath_ << std::endl;
        std::abort();
    }

    uint64_t energy_uj;
    energy_file >> energy_uj;
    float energy_uj_float = static_cast<float>(energy_uj);
    //check energy reg overflow
    if (energy_uj_float<0) {
        std::cerr << "Error reading energy file: " << trackPath_  << "energy < 0 , maybe overflow" << std::endl;
        return -1;
    }
    return energy_uj_float;  // Return energy in microjoules
}
