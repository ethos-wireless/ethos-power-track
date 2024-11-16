#include <Tracker.h>

Tracker::Tracker()
	: ID_(0),
	  trackPath_(""){
}

Tracker::~Tracker() {
    
}


void Tracker::SetTrackPath() {
    trackPath_ = "/sys/class/powercap/intel-rapl:0/energy_uj";
}
void Tracker::SetTrackPath(std::string track_path) {
    trackPath_ = track_path;
}
std::string Tracker::GetTrackPath() const {
    return trackPath_;
}

void Tracker::SetResult(float result){
    measuredResult_ = result;
}
float Tracker::GetResult() const{
    return measuredResult_ ;
}