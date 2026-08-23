# SocketCAN: Software-Only CAN/CAN FD Communication on Linux

**A Complete Implementation Guide for Ubuntu 22.04.5 LTS**

---

## 📋 Table of Contents

1. [Project Overview](#project-overview)
2. [Prerequisites](#prerequisites)
3. [Quick Start](#quick-start)
4. [Understanding SocketCAN](#understanding-socketcan)
5. [System Architecture](#system-architecture)
6. [CAN Message Design](#can-message-design)
7. [Implementation Details](#implementation-details)
8. [Learning Challenges](#learning-challenges)
9. [Testing and Demonstration](#testing-and-demonstration)
10. [GitHub Upload Guide](#github-upload-guide)

---

## 🎯 Project Overview

This project demonstrates **software-only CAN and CAN FD communication** using the Linux SocketCAN framework. Instead of requiring physical CAN hardware, it uses **virtual CAN interfaces (vcan0)** to enable multiple software nodes to communicate over a simulated CAN bus.

### Key Features

- ✅ No physical CAN hardware required
- ✅ Multiple ECU nodes (Vehicle, Dashboard, Logger)
- ✅ CAN message filtering and routing
- ✅ Real-time traffic logging to CSV
- ✅ Node failure detection with diagnostics
- ✅ CAN FD support (up to 64-byte payloads)
- ✅ Full source code in one file

### What You'll Learn

- SocketCAN architecture and design
- Virtual CAN interface creation and usage
- CAN message encoding/decoding
- Multi-application CAN networking
- Message filtering techniques
- Timeout-based fault detection
- CAN FD capabilities and benefits

---

## 📦 Prerequisites

### Ubuntu 22.04.5 LTS System

```bash
# Verify Ubuntu version
lsb_release -a
# Output should show: Ubuntu 22.04.5 LTS
```

### Required Packages

```bash
# Update package list
sudo apt update

# Install required packages
sudo apt install build-essential can-utils iproute2
```

### Verify Installations

```bash
# Check compiler
gcc --version

# Check can-utils utilities
candump --help
cansend --help

# Check ip command
ip --version
```

---

## 🚀 Quick Start

### Step 1: Load Virtual CAN Module

```bash
# Load vcan (Virtual CAN) kernel module
sudo modprobe vcan

# Verify vcan module loaded
lsmod | grep vcan
# Output: vcan  <size>  0
```

### Step 2: Create Virtual CAN Interface

```bash
# Create vcan0 interface
sudo ip link add dev vcan0 type vcan

# Activate vcan0
sudo ip link set up vcan0

# Verify vcan0 is UP
ip link show vcan0
# Output should show: vcan0: <NOARP,UP,LOWER_UP>
```

### Step 3: Download and Compile

```bash
# Create project directory
mkdir -p ~/socketcan_project
cd ~/socketcan_project

# Copy the provided socketcan_demo.c file here

# Compile
gcc -Wall -Wextra -O2 socketcan_demo.c -o socketcan_demo -lm
```

### Step 4: Run the System

**Terminal 1 - Logger:**
```bash
cd ~/socketcan_project
./socketcan_demo logger can_log.csv
```

**Terminal 2 - Dashboard:**
```bash
cd ~/socketcan_project
./socketcan_demo dashboard all
```

**Terminal 3 - Vehicle ECU:**
```bash
cd ~/socketcan_project
./socketcan_demo vehicle
```

**Terminal 4 - Monitor (optional):**
```bash
candump vcan0
```

### Expected Output

**Dashboard Terminal:**
```
--------------------------------
        Vehicle Dashboard
--------------------------------
Speed       : 56 km/h
Engine RPM  : 2760 rpm
Temperature : 73 C
--------------------------------
Vehicle ECU : ONLINE
--------------------------------
```

**Logger Terminal:**
```
Logger ECU started.
Logging to can_log.csv

[1750000000.123456] ID=0x100 DLC=2 Data=38 00
[1750000000.123500] ID=0x101 DLC=2 Data=DC 03
[1750000000.123530] ID=0x102 DLC=2 Data=49 00
```

---

## 🔍 Understanding SocketCAN

### What is SocketCAN?

SocketCAN is the Linux kernel's standard framework for CAN communication. It treats CAN as a **network interface**, similar to Ethernet (eth0) or WiFi (wlan0).

```
Traditional Approach:        SocketCAN Approach:
┌──────────────┐            ┌──────────────┐
│ Your App     │            │ Your App     │
└──────┬───────┘            └──────┬───────┘
       │                           │
    Vendor API                   Socket API
       │                           │
   Vendor Driver              SocketCAN
       │                           │
   CAN Hardware              CAN (any hw)
```

### Why Network Interface?

Linux provides **unified abstraction** for different hardware types:
- Ethernet → eth0
- WiFi → wlan0
- CAN → can0, vcan0

This gives SocketCAN applications:
- **Portability**: Works with any CAN adapter
- **Standard APIs**: Uses familiar socket programming
- **Tool Integration**: Works with candump, cansend, etc.
- **No Vendor Lock-in**: Switch hardware without code changes

### SocketCAN vs Vendor APIs

| Aspect | Vendor API | SocketCAN |
|--------|-----------|-----------|
| Portability | Low (vendor-specific) | High (standard Linux) |
| Learning Curve | Vendor documentation | Standard socket APIs |
| Multi-vendor | Each vendor different API | Single code works everywhere |
| Tools | Vendor-provided | Linux utilities (candump, etc.) |
| Maintenance | Vendor updates | Linux kernel updates |

---

## 🏗️ System Architecture

### Overall Architecture

```
                    Ubuntu 22.04.5 LTS
                    ──────────────────

                       SocketCAN
                           |
                        vcan0
                           |
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
    ┌───────────┐    ┌───────────┐    ┌──────────┐
    │ Vehicle   │    │ Dashboard │    │  Logger  │
    │   ECU     │    │    ECU    │    │   ECU    │
    └─────┬─────┘    └─────┬─────┘    └────┬─────┘
          │ TX              │ RX             │ RX
          │                 │                │
          │    Speed/RPM/   │           Records
          └────Temp────────→│ Display       Traffic
                             │           ↓
                             └─→   can_log.csv
```

### Node Responsibilities

**Vehicle ECU (Transmitter)**
- Generates realistic vehicle data:
  - Speed: 0-120 km/h (varies realistically)
  - RPM: 800-5000 rpm (follows speed)
  - Temperature: 20-120°C (gradual changes)
- Transmits 3 CAN messages every 500ms
- Uses CAN IDs: 0x100, 0x101, 0x102

**Dashboard ECU (Receiver)**
- Receives CAN messages
- Decodes speed, RPM, temperature
- Displays real-time updates
- Optional filtering (all, speed-only, rpm-only)
- Timeout-based offline detection
- Shows "WARNING: Vehicle ECU Offline" after 2 seconds without messages

**Logger ECU (Receiver)**
- Receives all CAN traffic
- Records to `can_log.csv`
- Fields: Timestamp, CAN ID, DLC, Payload
- Independent operation (doesn't affect other nodes)

### Virtual CAN Interface (vcan0)

vcan0 is a **virtual network interface** that simulates CAN hardware:
- Created entirely in software
- No CAN controller needed
- No transceiver needed
- No USB adapter needed
- Perfect for development and testing
- Kernel delivers messages to all registered sockets

---

## 📋 CAN Message Design

### Message Definition

Three CAN messages carry vehicle data:

| CAN ID | Signal | DLC | Encoding | Unit | Transmission |
|--------|--------|-----|----------|------|--------------|
| 0x100 | Vehicle Speed | 2 | uint16_t LE | km/h | 500ms |
| 0x101 | Engine RPM | 2 | uint16_t LE | rpm | 500ms |
| 0x102 | Coolant Temperature | 2 | uint16_t LE | °C | 500ms |

**LE** = Little-Endian (LSB first)

### Payload Encoding Examples

**Speed 65 km/h:**
```
Value: 65 (decimal)
Hex: 0x0041
CAN Data: [0x41, 0x00]  (little-endian)
```

**RPM 2450:**
```
Value: 2450 (decimal)
Hex: 0x0992
CAN Data: [0x92, 0x09]  (little-endian)
```

**Temperature 88°C:**
```
Value: 88 (decimal)
Hex: 0x0058
CAN Data: [0x58, 0x00]  (little-endian)
```

### CAN Frame Structure (Classical CAN)

```
┌─────────────────────────────────┐
│   CAN Identifier (11-bit)       │  Examples: 0x100, 0x101, 0x102
├─────────────────────────────────┤
│   Data Length Code (DLC)        │  0-8 bytes for Classical CAN
├─────────────────────────────────┤
│   Data Payload (0-8 bytes)      │  Actual message data
├─────────────────────────────────┤
│   CRC & Frame Information       │  Handled by kernel/hardware
└─────────────────────────────────┘
```

---

## 💻 Implementation Details

### Key Source Code Sections

#### Creating SocketCAN Socket

```c
int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

struct sockaddr_can addr;
struct ifreq ifr;

strcpy(ifr.ifr_name, "vcan0");
ioctl(sock, SIOCGIFINDEX, &ifr);

addr.can_family = AF_CAN;
addr.can_ifindex = ifr.ifr_ifindex;

bind(sock, (struct sockaddr *)&addr, sizeof(addr));
```

#### Transmitting CAN Frame (Vehicle ECU)

```c
struct can_frame frame;
frame.can_id = 0x100;        // Speed message
frame.can_dlc = 2;           // 2 bytes

uint16_t speed_value = 65;   // 65 km/h

frame.data[0] = speed_value & 0xFF;        // LSB
frame.data[1] = (speed_value >> 8) & 0xFF; // MSB

write(sock, &frame, sizeof(frame));
```

#### Receiving and Decoding (Dashboard ECU)

```c
struct can_frame frame;
int nbytes = read(sock, &frame, sizeof(frame));

if (frame.can_id == 0x100) {  // Speed message
    int speed = frame.data[0] | (frame.data[1] << 8);
    // Use speed value
}
```

#### Message Filtering

```c
#include <linux/can/raw.h>

struct can_filter filter;
filter.can_id = 0x100;           // Speed only
filter.can_mask = CAN_SFF_MASK;

setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));
```

#### Timeout-Based Offline Detection

```c
fd_set readfds;
struct timeval timeout;

FD_ZERO(&readfds);
FD_SET(sock, &readfds);

timeout.tv_sec = 2;      // 2-second timeout
timeout.tv_usec = 0;

int result = select(sock + 1, &readfds, NULL, NULL, &timeout);

if (result == 0) {
    printf("WARNING: Vehicle ECU Offline\n");
}
```

#### CAN FD Transmission

```c
#include <linux/can/raw.h>

int enable_canfd = 1;
setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
           &enable_canfd, sizeof(enable_canfd));

struct canfd_frame frame;
frame.can_id = 0x300;
frame.len = 20;  // Up to 64 bytes

for (int i = 0; i < 20; i++) {
    frame.data[i] = i + 1;
}

write(sock, &frame, CANFD_MTU);
```

---

## 🧪 Testing and Demonstration

### Test Procedure

#### Step 1: Create vcan0
```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
ip link show vcan0
```

#### Step 2: Compile
```bash
cd ~/socketcan_project
gcc -Wall -Wextra -O2 socketcan_demo.c -o socketcan_demo -lm
```

#### Step 3: Run System (5 terminals)

**Terminal 1 - Setup:**
```bash
cd ~/socketass_project
# Just run once, let it stay open
```

**Terminal 2 - Logger:**
```bash
./logger_ecu
```

**Terminal 3 - Dashboard:**
```bash
./dashboard_ecu
./dashboard_diagnostic
```

**Terminal 4 - Vehicle:**
```bash
./vehicle_ecu
```

**Terminal 5 - Monitor:**
```bash
candump vcan0
# Shows: vcan0  100   [2]  20 00
#        vcan0  101   [2]  DC 03
#        vcan0  102   [2]  48 00
```

#### Step 4: Verify Outputs

**Check Logger File:**
```bash
cat can_log.csv | head -10
# Shows timestamps, IDs, DLC, payload
```

**Check Dashboard Display:**
```
Should continuously update Speed, RPM, Temperature
Should show "Vehicle ECU : ONLINE"
```

### Using Linux CAN Tools

#### candump - Monitor Traffic

```bash
# All traffic on vcan0
candump vcan0

# Speed messages only
candump vcan0,100:7FF

# RPM messages only
candump vcan0,101:7FF

# Output format:
# vcan0  100   [2]  38 00
#        ^     ^    ^^^
#        |     |    └─ Payload (little-endian)
#        |     └────── DLC (Data Length Code)
#        └──────────── CAN ID
```

#### cansend - Transmit Message

```bash
# Send speed message (65 km/h = 0x41, 0x00)
cansend vcan0 100#4100

# Send RPM message (2450 = 0x92, 0x09)
cansend vcan0 101#9209

# Send temperature (88°C = 0x58, 0x00)
cansend vcan0 102#5800

# Send unknown message
cansend vcan0 200#AABBCCDD
```

#### cangen - Generate Traffic

```bash
# Generate random CAN frames on vcan0
cangen vcan0

# Generate at specific rate (optional)
cangen -n 100 vcan0  # 100 frames then stop

# Stop with Ctrl+C
```

### Performance Testing

**Measure CPU Usage:**
```bash
# Monitor CPU while running
top

# Or use ps
ps aux | grep socketcan_demo
```

**Measure Log File Growth:**
```bash
# Before experiment
ls -lh can_log.csv

# After 60 seconds at different rates
# At 500ms: ~2-5 KB
# At 100ms: ~10-25 KB
# At 10ms: ~100-250 KB
```

**Count Messages:**
```bash
# Count lines in log file
wc -l can_log.csv

# At 500ms for 60 seconds: ~360 lines (6 msg/s × 60s)
# At 100ms for 60 seconds: ~1800 lines (30 msg/s × 60s)
```

---

## 📊 Quick Reference

### CAN Message IDs
- `0x100` = Speed (km/h)
- `0x101` = RPM (rpm)
- `0x102` = Temperature (°C)
- `0x200` = Unknown (test only)
- `0x300` = CAN FD test

### Configuration Parameters
- **Transmission Period**: 500ms (change `usleep()`)
- **Timeout Detection**: 2 seconds
- **Speed Range**: 0-120 km/h
- **RPM Range**: 800-5000 rpm
- **Temperature Range**: 20-120°C
- **CAN FD Payload**: 20 bytes (can extend to 64)

### Useful Commands
```bash
# Setup vcan
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

# Monitor traffic
candump vcan0

# Send message
cansend vcan0 100#4100

# Compile project
gcc -Wall -Wextra -O2 socketcan_demo.c -o socketcan_demo -lm

# Run nodes
./socketcan_demo vehicle
./socketcan_demo dashboard all
./socketcan_demo logger can_log.csv

# View log
cat can_log.csv
```

---

## 🎓 Learning Outcomes

After completing this project, you should be able to:

✅ **Explain SocketCAN architecture** - How it differs from vendor APIs  
✅ **Create virtual CAN networks** - Using vcan without physical hardware  
✅ **Develop CAN sender/receiver applications** - Using Linux socket APIs  
✅ **Implement message filtering** - Reducing kernel overhead  
✅ **Log and analyze CAN traffic** - Creating and parsing CSV logs  
✅ **Detect node failures** - Using timeout-based diagnostics  
✅ **Use CAN FD** - Extending payloads beyond 8 bytes  
✅ **Design CAN message structures** - Encoding signals into frames  
✅ **Troubleshoot CAN systems** - Using Linux tools and utilities  
✅ **Deploy to GitHub** - Version control and sharing  

---

## ✍️ Author Notes

This implementation demonstrates core SocketCAN concepts using practical, production-ready code. The single-file design makes it ideal for learning and embedding in other projects. The multi-node architecture shows how real CAN systems scale from simple point-to-point communication to complex networked systems.

The timeout-based diagnostics provide a lightweight alternative to explicit heartbeat messages, demonstrating practical engineering trade-offs in embedded systems design.

---



