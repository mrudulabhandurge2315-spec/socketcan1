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

## 🧪 Learning Challenges

### Challenge 1: Traffic Observation

**Objective:** Verify multiple applications receive the same CAN traffic

**Setup:**
```bash
Terminal 1: ./socketcan_demo logger can_log.csv
Terminal 2: ./socketcan_demo dashboard all
Terminal 3: ./socketcan_demo vehicle
```

**Expected Results:**
- ✅ Dashboard displays vehicle data
- ✅ Logger records traffic to CSV
- ✅ Logger activity doesn't affect Dashboard
- ✅ Vehicle ECU continues transmitting
- ✅ All three nodes communicate seamlessly

**Findings:**
Multiple SocketCAN sockets receive matching frames from the same interface. Kernel efficiently delivers to all registered receivers. This is a fundamental capability of SocketCAN.

---

### Challenge 2: Message Filtering

**Objective:** Receive only specific CAN messages

**Test Speed Messages Only:**
```bash
# Terminal 1
./socketcan_demo dashboard speed

# Terminal 2
./socketcan_demo vehicle
```

**Test RPM Messages Only:**
```bash
# Terminal 1
./socketcan_demo dashboard rpm

# Terminal 2
./socketcan_demo vehicle
```

**Test All Messages:**
```bash
# Terminal 1
./socketcan_demo dashboard all

# Terminal 2
./socketcan_demo vehicle
```

**Expected Results:**
- ✅ Filtering at kernel level (CAN_RAW_FILTER socket option)
- ✅ Reduces unnecessary message delivery to application
- ✅ Dashboard only updates when filtered message received
- ✅ Multiple filters can be active simultaneously

---

### Challenge 3: Unknown Message Detection

**Objective:** Send a CAN message not in the system definition

**Send Unknown Message:**
```bash
# Terminal 1
./socketcan_demo unknown

# Terminal 2  
./socketcan_demo dashboard all

# Terminal 3
./socketcan_demo logger can_log.csv
```

**Expected Results:**
- ✅ Logger records 0x200 message (receives all traffic)
- ✅ Dashboard ignores 0x200 (not in definition)
- ✅ Dashboard continues working normally
- ✅ System stable with unknown message IDs

**Findings:**
This demonstrates importance of explicit message filtering. Production systems should only process known message types.

---

### Challenge 4: Transmission Rate Study

**Default Rate:** 500ms (2 messages/second per signal = 6 total/second)

**Experiment with Different Rates:**

Edit `socketcan_demo.c` and change `usleep()` parameter:

```c
// Original (500ms):
usleep(500000);

// Experiment 1: 1-second interval (slowest)
usleep(1000000);

// Experiment 2: 100ms interval (10 msg/s)
usleep(100000);

// Experiment 3: 10ms interval (100 msg/s - very high)
usleep(10000);
```

**Expected Observations:**

| Interval | Frequency | Dashboard | Log Growth | CPU |
|----------|-----------|-----------|-----------|-----|
| 1000ms | Slow | Slow updates | Minimal | Low |
| 500ms | Normal | Good updates | Moderate | Low |
| 100ms | Fast | Smooth | Significant | Moderate |
| 10ms | Very Fast | Rapid | Large | High |

**Findings:**
- Higher transmission rates → smoother display
- Higher transmission rates → faster file growth
- No crashes at any reasonable rate
- Kernel efficiently handles high message volume

---

### Challenge 5: Node Failure Study

**Objective:** Test system behavior when Vehicle ECU terminates

**Setup:**

Terminal 1:
```bash
./socketcan_demo logger can_log.csv
```

Terminal 2:
```bash
./socketcan_demo dashboard all
```

Terminal 3:
```bash
./socketcan_demo vehicle
# After 10 seconds, press Ctrl+C to stop Vehicle ECU
```

**Expected Results:**

**Before stopping Vehicle ECU:**
```
Dashboard shows:
Speed       : 56 km/h
Engine RPM  : 2760 rpm
Temperature : 73 C
Vehicle ECU : ONLINE
```

**After stopping Vehicle ECU (wait 2 seconds):**
```
Dashboard shows:
Speed       : 56 km/h           (last received value)
Engine RPM  : 2760 rpm          (last received value)
Temperature : 73 C              (last received value)
WARNING: Vehicle ECU Offline    (timeout triggered)
```

**Logger Results:**
```
Logger stops receiving new messages
can_log.csv has all messages until Vehicle ECU was stopped
```

**Findings:**
- ✅ Dashboard detects failure via 2-second timeout
- ✅ No automatic recovery (expected behavior)
- ✅ Last known values remain displayed
- ✅ Clear warning message indicates problem

---

### Challenge 6: CAN FD Exploration

**Objective:** Demonstrate CAN FD's larger payload capability

**Setup:**

Terminal 1:
```bash
./socketcan_demo canfd_rx
```

Terminal 2:
```bash
./socketcan_demo canfd_tx
```

**Expected Output:**

Terminal 2 sends 20-byte CAN FD frame:
```
CAN FD Sender
Sending 20-byte CAN FD frame
CAN ID : 0x300
Data   : 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14
```

Terminal 1 receives:
```
CAN FD Receiver started.

Received CAN FD frame
CAN ID: 0x300
Length: 20 bytes
Data: 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14
```

### CAN vs CAN FD Comparison

| Feature | Classical CAN | CAN FD |
|---------|---------------|--------|
| Max Payload | 8 bytes | 64 bytes |
| Arbitration Phase | Standard bit rate | Standard bit rate |
| Data Phase | Same as arbitration | Can use faster bit rate |
| Frame Structure | struct can_frame | struct canfd_frame |
| Linux Support | Full | Full |
| Kernel Module Setup | CAN_RAW | CAN_RAW_FD_FRAMES |

**When to Use CAN FD:**
- ✅ High-bandwidth data (video, lidar)
- ✅ Consolidating multiple CAN messages into one
- ✅ Complex diagnostic data
- ✅ Automotive systems requiring faster rates

**Benefits Demonstrated:**
- ✅ 8× larger payload (64 vs 8 bytes)
- ✅ Variable bit rates reduce latency
- ✅ Backward compatible with classical CAN
- ✅ Full Linux kernel support

---

### Challenge 7: Basic Diagnostics

**Objective:** Detect loss of communication and warn user

**Implementation:** Timeout-based detection

**How It Works:**
```
Vehicle ECU sends Speed message every 500ms
         ↓
Dashboard starts timer
         ↓
New message received? → Reset timer
         ↓
No message for 2 seconds? → Display WARNING
```

**Testing:**

Run dashboard:
```bash
./socketcan_demo dashboard all
```

Dashboard shows:
```
Vehicle ECU : ONLINE
```

Stop Vehicle ECU (Ctrl+C), wait 2 seconds:
```
WARNING: Vehicle ECU Offline
```

Restart Vehicle ECU:
```
Vehicle ECU : ONLINE
```

**Features:**
- ✅ Non-intrusive (doesn't require heartbeat message)
- ✅ Configurable timeout (2 seconds in example)
- ✅ Automatic recovery detection
- ✅ Works with existing messages

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
cd ~/socketcan_project
# Just run once, let it stay open
```

**Terminal 2 - Logger:**
```bash
./socketcan_demo logger can_log.csv
```

**Terminal 3 - Dashboard:**
```bash
./socketcan_demo dashboard all
```

**Terminal 4 - Vehicle:**
```bash
./socketcan_demo vehicle
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

## 📤 GitHub Upload Guide

### Step 1: Initialize Local Git Repository

```bash
cd ~/socketcan_project

# Initialize git
git init

# Configure git (use your GitHub username/email)
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Verify configuration
git config --list
```

### Step 2: Create .gitignore File

```bash
cat > .gitignore << 'EOF'
# Compiled binaries
socketcan_demo
vehicle_ecu
dashboard_ecu
logger_ecu
unknown_sender
canfd_sender
canfd_receiver
*.o
*.a
*.so

# Generated files
can_log.csv
*.swp
*.swo
*~

# IDE
.vscode/
.idea/
*.code-workspace

# OS
.DS_Store
Thumbs.db
EOF

git add .gitignore
git commit -m "Add .gitignore"
```

### Step 3: Prepare Files

**Organize Your Project:**
```bash
mkdir -p docs
mkdir -p screenshots
mkdir -p tests

# Copy technical report
cp SocketCAN_Technical_Report.docx docs/

# Create comprehensive README (this file)
# Already done as README.md

# Add any test results or screenshots
# Copy screenshots to screenshots/
```

### Step 4: Create Project Structure

```bash
.
├── socketcan_demo.c              # Main implementation
├── README.md                      # This file
├── LICENSE                        # Add license
├── .gitignore
│
├── docs/
│   └── SocketCAN_Technical_Report.docx
│   └── SETUP.md                  # Setup instructions
│   └── CHALLENGES.md             # Challenge details
│
├── screenshots/
│   ├── 01_vcan0_setup.png
│   ├── 02_candump_traffic.png
│   ├── 03_dashboard_running.png
│   ├── 04_logger_output.png
│   └── README.md
│
└── tests/
    ├── test_single_ecu.sh
    ├── test_all_nodes.sh
    └── test_rates.sh
```

### Step 5: Add LICENSE File

```bash
cat > LICENSE << 'EOF'
MIT License

Copyright (c) 2026 SocketCAN Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
EOF

git add LICENSE
git commit -m "Add MIT License"
```

### Step 6: Add All Files to Git

```bash
git add README.md
git add socketcan_demo.c
git add .gitignore
git add docs/
git add screenshots/
git add tests/

# Check status
git status

# Should show all files added
```

### Step 7: Create Initial Commit

```bash
git commit -m "Initial commit: SocketCAN software implementation

- Complete single-file implementation
- Vehicle, Dashboard, and Logger ECUs
- CAN message filtering
- CAN FD support
- Timeout-based diagnostics
- Comprehensive documentation"
```

### Step 8: Create GitHub Repository

**On GitHub.com:**
1. Click **+** → **New repository**
2. Name it: `socketcan-demo` or similar
3. Description: "Software-only CAN/CAN FD communication using Linux SocketCAN"
4. Choose visibility: **Public** (for sharing) or **Private**
5. Do NOT initialize with README (you already have one)
6. Click **Create repository**

### Step 9: Connect Local Repository to GitHub

```bash
# Add remote origin (replace USERNAME and REPO-NAME)
git remote add origin https://github.com/USERNAME/socketcan-demo.git

# Verify remote
git remote -v
# Output should show:
# origin  https://github.com/USERNAME/socketcan-demo.git (fetch)
# origin  https://github.com/USERNAME/socketcan-demo.git (push)
```

### Step 10: Push to GitHub

```bash
# Rename branch to main (modern standard)
git branch -M main

# Push to GitHub
git push -u origin main

# First push uses -u to set upstream
# Subsequent pushes can use: git push
```

### Step 11: Verify on GitHub

Visit: `https://github.com/USERNAME/socketcan-demo`

You should see:
- ✅ All files uploaded
- ✅ README.md displayed
- ✅ File tree visible
- ✅ Commit history

### Common Git Commands After Upload

```bash
# Check status
git status

# Add modified files
git add socketcan_demo.c

# Commit changes
git commit -m "Fix: improve error handling"

# Push to GitHub
git push

# View log
git log --oneline

# Create branch for experiments
git checkout -b feature/new-feature

# Switch branches
git checkout main

# Merge branch
git merge feature/new-feature

# Delete branch
git branch -d feature/new-feature
```

### Troubleshooting

**"fatal: not a git repository"**
```bash
cd ~/socketcan_project
git init
```

**"permission denied" when pushing**
```bash
# Use SSH instead of HTTPS
git remote set-url origin git@github.com:USERNAME/socketcan-demo.git

# Or configure GitHub credentials via HTTPS
# GitHub → Settings → Personal access tokens → Generate new token
```

**Already have local repo, want to push to GitHub**
```bash
git remote add origin https://github.com/USERNAME/socketcan-demo.git
git branch -M main
git push -u origin main
```

### Creating README Badges (Optional)

Add to top of README.md:

```markdown
# SocketCAN: Software-Only CAN/CAN FD Communication

[![GitHub](https://img.shields.io/badge/GitHub-socketcan--demo-blue)](https://github.com/USERNAME/socketcan-demo)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-orange)](https://www.linux.org/)
[![Ubuntu 22.04](https://img.shields.io/badge/Ubuntu-22.04.5%20LTS-E95420)](https://ubuntu.com/)

**A Complete Implementation Guide for Ubuntu 22.04.5 LTS**
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

## 📚 Additional Resources

### Linux Kernel Documentation
- [SocketCAN Linux Kernel Docs](https://www.kernel.org/doc/html/latest/networking/can.html)
- [can-utils GitHub Repository](https://github.com/linux-can/can-utils)

### CAN Protocol Resources
- CAN 2.0 Specification (ISO 11898-1)
- CAN FD Specification (ISO 11898-2)
- Bosch CAN Protocol Description

### Useful Tools
- `candump` - Monitor CAN traffic
- `cansend` - Transmit CAN frames
- `cangen` - Generate CAN traffic
- `candatype` - Dump CAN data types

---

## 📝 Notes

- All timestamps use Unix epoch format (seconds.microseconds)
- CAN messages use little-endian encoding for multi-byte values
- Maximum timeout detection is limited by select() precision
- Virtual CAN is not a replacement for testing with real hardware
- All code is GPL/MIT licensed

---

## ✍️ Author Notes

This implementation demonstrates core SocketCAN concepts using practical, production-ready code. The single-file design makes it ideal for learning and embedding in other projects. The multi-node architecture shows how real CAN systems scale from simple point-to-point communication to complex networked systems.

The timeout-based diagnostics provide a lightweight alternative to explicit heartbeat messages, demonstrating practical engineering trade-offs in embedded systems design.

---

**Happy CAN Networking! 🚀**

For questions or contributions, create an issue on GitHub.

