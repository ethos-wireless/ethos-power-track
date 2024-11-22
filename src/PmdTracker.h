#ifndef PMDTRACKER_H
#define PMDTRACKER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <mutex>
#include <memory>

#include <Tracker.h>
#include <CppLinuxSerial/SerialPort.hpp>
using namespace mn::CppLinuxSerial;

static constexpr uint8_t PMD_USB_VID = 0xEE;
static constexpr uint8_t PMD_USB_PID = 0x0A;
static constexpr double RX_TIMEOUT_MS = 2000;

// Overload to be able to print vectors to std::cout
template <typename T>
std::ostream& operator<<( std::ostream& ostrm, const std::vector<T>& vec ){
    if (vec.size() == 0) {
        return ostrm << "[]";
    }
    for( int j = 0, n = vec.size(); j < n; ++j ){
        ostrm << ",["[ !j ] << " " << vec[ j ];
    }
    return ostrm << " ]";
}

class PmdTracker : public Tracker {
public:
  PmdTracker();
  PmdTracker(uint32_t ID, std::string path, std::string port, size_t speed);
  virtual ~PmdTracker() override;

  void PmdConfigRetryFindDevice(std::vector<uint8_t>& rxDataBinary);
  void PmdConfigReopenPort();
  void PmdConfigTxStart();
  void PmdConfigTxStop();
  std::vector<float> GetPmdPower();
  void SetTrackPath() override;
  void SetTrackPath(std::string path);


// UART interface commands
enum UART_CMD : uint8_t
{
    UART_CMD_WELCOME,
    UART_CMD_READ_ID,
    UART_CMD_READ_SENSORS,
    UART_CMD_READ_SENSOR_VALUES,
    UART_CMD_READ_CONFIG,
    UART_CMD_WRITE_CONFIG,
    UART_CMD_READ_ADC,
    UART_CMD_WRITE_CONFIG_CONT_TX,
    UART_CMD_WRITE_CONFIG_UART,
    UART_CMD_RESET = 0xF0,
    UART_CMD_BOOTLOADER = 0xF1,
    UART_CMD_NOP = 0xFF
};

private:
  std::string trackPath_;
  uint32_t ID_;

  std::string port_;
  size_t speed_;
//   std::atomic<bool> dev_attached_; 
  bool dev_attached_; 

  SerialPort serial_port_;
  size_t pmd_id_;

  std::vector<uint8_t> rx_buffer_;

};


#endif /* PMDTRACKER_H */