# !/bin/bash

if [ -r "/sys/class/powercap/intel-rapl:0/energy_uj" ]; then
  echo "energy_uj has read permissions."
else
  echo "energy_uj does not have read permissions."
  sudo -E chmod a+r /sys/class/powercap/intel-rapl:0/energy_uj
  echo "energy_uj added read permissions."
fi

mkdir -p build && cd build
cmake .. && cmake --build .