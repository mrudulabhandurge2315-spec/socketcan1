# SocketCAN: Software-Only CAN / CAN FD Communication on Linux

**Project Domain:** Automotive Communication Networks  
**Platform:** Ubuntu Linux / WSL2  
**Programming Language:** Embedded C / C  
**Framework:** Linux SocketCAN  
**Network Interface:** Virtual CAN (`vcan0`)  
**Hardware Requirement:** No external CAN hardware required

---

## 1. Project Overview

This project demonstrates CAN (Controller Area Network) and CAN FD (CAN with Flexible Data-Rate) communication using the Linux SocketCAN framework.

The implementation uses a virtual CAN interface (`vcan0`) to simulate a CAN network without requiring physical CAN controllers, transceivers or USB-CAN adapters.

Multiple software-based Electronic Control Units (ECUs) communicate over the same virtual CAN network.

The project consists of:

- **Vehicle ECU:** Generates vehicle parameters and transmits CAN messages.
- **Dashboard ECU:** Receives CAN messages, decodes the data and displays vehicle information.
- **Logger ECU:** Monitors CAN traffic and stores received messages in a CSV file.
- **Diagnostic ECU:** Detects communication loss using message timeout monitoring.
- **Unknown Message Sender:** Transmits additional CAN messages to test unknown-message handling.
- **CAN FD Sender and Receiver:** Demonstrate communication using CAN FD frames.

### Project Objectives

1. Understand the Linux SocketCAN architecture.
2. Create a software-only CAN network using `vcan0`.
3. Implement CAN message transmission and reception.
4. Develop multiple independent software ECU nodes.
5. Implement CAN message filtering.
6. Record and analyze CAN traffic.
7. Detect ECU communication failures.
8. Explore CAN FD communication and its advantages.

## 2. Technologies and Tools Used

| Technology | Purpose |
|---|---|
| C Programming | ECU application development |
| Linux SocketCAN | CAN communication framework |
| Virtual CAN (`vcan0`) | Software-based CAN network |
| GCC | Compilation of C programs |
| Linux Socket API | CAN frame transmission and reception |
| can-utils | CAN traffic monitoring and testing |
| WSL2 Ubuntu | Development and execution environment |
| CSV | CAN traffic logging |

### Prerequisites

Install the required packages:

```bash
sudo apt update
sudo apt install build-essential can-utils iproute2
```

Verify the installations:

```bash
gcc --version
candump --help
cansend --help
ip --version
```

## 3. Understanding SocketCAN

SocketCAN is the Linux kernel framework that provides CAN communication through the standard Linux networking architecture.

Instead of accessing CAN hardware through a vendor-specific API, applications communicate using standard socket programming functions.

### Why is CAN treated as a network interface?

Linux represents CAN interfaces similarly to Ethernet and Wi-Fi interfaces.

Examples:

- Ethernet: `eth0`
- Wi-Fi: `wlan0`
- Physical CAN: `can0`
- Virtual CAN: `vcan0`

This provides a common interface for applications to transmit, receive, filter and monitor CAN messages.

### SocketCAN Advantages

- Standard Linux socket programming interface.
- Supports different CAN hardware manufacturers.
- Multiple applications can access the same CAN interface.
- Provides message filtering.
- Supports Classical CAN and CAN FD.
- Integrates with Linux networking tools.
- Enables software development without physical CAN hardware through Virtual CAN.

## 4. System Architecture

The project uses a virtual CAN network to connect multiple software-based ECUs.

```text
                 Ubuntu Linux / WSL2
                         |
                    SocketCAN
                         |
                      vcan0
                         |
             +-----------+-----------+
             |                       |
        Vehicle ECU              CAN Network
         (Sender)                    |
             |              +--------+--------+
             |              |                 |
        CAN Messages    Dashboard ECU     Logger ECU
                        (Receiver)        (Receiver)
                            |                 |
                      Live Display       can_log.csv
                            |
                      Vehicle Data
```

Additional test applications:

- Diagnostic ECU monitors missing vehicle messages.
- Unknown Message Sender introduces unrecognized CAN identifiers.
- CAN FD Sender and Receiver demonstrate extended payload communication.

### ECU Responsibilities

**Vehicle ECU**

Generates and transmits:

- Vehicle Speed: 0–120 km/h
- Engine RPM: 800–5000 rpm
- Coolant Temperature: 20–120 °C

**Dashboard ECU**

- Receives CAN frames.
- Identifies messages using CAN IDs.
- Decodes the received payload.
- Displays vehicle information continuously.
- Supports message filtering as implemented in the application.

**Logger ECU**

- Receives CAN traffic independently.
- Records timestamps, CAN identifiers, payload lengths and data.
- Stores the captured information in `can_log.csv`.

**Diagnostic ECU**

- Monitors the arrival of vehicle messages.
- Uses a timeout mechanism to identify communication loss.
- Displays an offline warning when the expected message is not received within the configured period.

## 5. CAN Message Definitions

The vehicle information is transmitted using separate CAN messages.

| CAN ID | Signal | Data Length | Unit |
|---|---|---:|---|
| 0x100 | Vehicle Speed | 2 bytes | km/h |
| 0x101 | Engine RPM | 2 bytes | rpm |
| 0x102 | Coolant Temperature | 2 bytes | °C |

**Encoding:** 16-bit unsigned integer, little-endian, as described in the initial message design.

### Example: CAN Payload Encoding

**Vehicle Speed = 65 km/h**

```text
Decimal: 65
Hexadecimal: 0x0041
CAN Data: 41 00
```

**Engine RPM = 2450 rpm**

```text
Decimal: 2450
Hexadecimal: 0x0992
CAN Data: 92 09
```

**Coolant Temperature = 88 °C**

```text
Decimal: 88
Hexadecimal: 0x0058
CAN Data: 58 00
```

### Classical CAN Frame

A Classical CAN frame contains:

- CAN Identifier: identifies the message.
- DLC: indicates the payload length (0–8 bytes).
- Data: contains the actual information.
- Control, CRC and other protocol fields: used for frame control and error detection.

The application works with the CAN identifier, DLC and data payload through SocketCAN.

### Standard and Extended Identifiers

| Type | Identifier Length |
|---|---:|
| Standard CAN | 11 bits |
| Extended CAN | 29 bits |

## 6. Project File Structure

The project uses separate source files for individual ECU applications.

```text
socketass_project/
│
├── vehicle_ecu.c
├── dashboard_ecu.c
├── logger_ecu.c
├── dashboard_diagnostic.c
├── unknown_sender.c
├── canfd_sender.c
├── canfd_receiver.c
│
├── vehicle_ecu
├── dashboard_ecu
├── logger_ecu
├── dashboard_diagnostic
├── unknown_sender
├── canfd_sender
├── canfd_receiver
│
├── can_log.csv
├── README.md
└── .gitignore
```

**Source files:**

| File | Description |
|---|---|
| `vehicle_ecu.c` | Vehicle parameter generation and CAN transmission |
| `dashboard_ecu.c` | CAN reception and dashboard display |
| `logger_ecu.c` | CAN traffic recording |
| `dashboard_diagnostic.c` | Communication timeout and failure detection |
| `unknown_sender.c` | Unknown CAN message transmission |
| `canfd_sender.c` | CAN FD message transmission |
| `canfd_receiver.c` | CAN FD message reception |

The executable files are generated by compiling their corresponding C source files.

## 7. Implementation and Execution

### Step 1: Create the Virtual CAN Interface

Load the required kernel modules:

```bash
sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan
```

Create the virtual CAN interface:

```bash
sudo ip link add dev vcan0 type vcan
```

Activate the interface:

```bash
sudo ip link set up vcan0
```

Verify:

```bash
ip link show vcan0
```

Expected interface information:

```text
vcan0: <NOARP,UP,LOWER_UP>
link/can
```

If `vcan0` already exists, do not create it again. Simply activate it:

```bash
sudo ip link set up vcan0
```

### Step 2: Compile the ECU Applications

Navigate to the project directory:

```bash
cd ~/socketass_project
```

Compile the individual applications:

```bash
gcc -Wall -Wextra vehicle_ecu.c -o vehicle_ecu
gcc -Wall -Wextra dashboard_ecu.c -o dashboard_ecu
gcc -Wall -Wextra logger_ecu.c -o logger_ecu
gcc -Wall -Wextra dashboard_diagnostic.c -o dashboard_diagnostic
gcc -Wall -Wextra unknown_sender.c -o unknown_sender
gcc -Wall -Wextra canfd_sender.c -o canfd_sender
gcc -Wall -Wextra canfd_receiver.c -o canfd_receiver
```

If a source file requires an additional library, include the appropriate linker option.

### Step 3: Run the Main CAN Demonstration

Use separate Ubuntu terminal windows.

**Terminal 1 – Logger ECU**

```bash
cd ~/socketass_project
./logger_ecu
```

The logger records CAN messages in:

```text
can_log.csv
```

**Terminal 2 – Dashboard ECU**

```bash
cd ~/socketass_project
./dashboard_ecu
```

**Terminal 3 – Vehicle ECU**

```bash
cd ~/socketass_project
./vehicle_ecu
```

**Terminal 4 – CAN Traffic Monitor**

```bash
candump vcan0
```

This terminal provides an independent view of CAN traffic on the virtual interface.

### Step 4: Verify the Logger Output

```bash
cd ~/socketass_project
head can_log.csv
```

The log file should contain the captured CAN message information.

## 8. Learning Challenges and Experiments

### Challenge 1: Traffic Observation

**Objective:** Observe CAN messages generated by the Vehicle ECU.

Run the Vehicle ECU, Dashboard ECU and Logger ECU simultaneously.

Use:

```bash
candump vcan0
```

Observations:

- Multiple applications can independently receive the same CAN traffic.
- Activating the Logger ECU does not prevent the Dashboard ECU from receiving messages.
- Messages are distributed to the applications according to their socket subscriptions and filters.
- The virtual CAN interface enables communication without physical CAN hardware.

### Challenge 2: CAN Message Filtering

**Objective:** Receive selected CAN messages.

The application can use the SocketCAN raw filtering mechanism.

Relevant Linux API:

```c
setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));
```

Example CAN filters:

| Filter | CAN ID | Purpose |
|---|---|---|
| Speed | 0x100 | Receive speed messages |
| RPM | 0x101 | Receive RPM messages |
| Temperature | 0x102 | Receive temperature messages |

Filtering reduces the number of irrelevant messages delivered to the application.

### Challenge 3: Unknown Message Detection

**Objective:** Introduce an additional CAN message.

Run:

```bash
./unknown_sender
```

An independent test message can also be transmitted using:

```bash
cansend vcan0 200#AABBCCDD
```

Verify:

- Whether the Logger ECU records the unknown message.
- Whether the Dashboard ECU ignores or handles it.
- Whether the application continues operating normally.

An unknown CAN identifier does not automatically cause a communication failure. Its handling depends on the receiver's application logic and configured filters.

### Challenge 4: Transmission Rate Study

**Objective:** Observe the effect of different message transmission periods.

The transmission period can be adjusted in the Vehicle ECU source code.

Suggested test intervals:

| Transmission Period | Expected Effect |
|---|---|
| 500 ms | Lower message frequency |
| 100 ms | Higher message frequency |
| 10 ms | Much higher message frequency |

Measure:

- Dashboard update behavior.
- Logger file growth.
- Number of received messages.
- CPU usage.

Useful commands:

```bash
ls -lh can_log.csv
wc -l can_log.csv
top
```

Record actual experimental results rather than assuming fixed performance values.

### Challenge 5: Node Failure Study

**Objective:** Investigate communication loss.

1. Start the Logger ECU.
2. Start the Dashboard ECU.
3. Start the Vehicle ECU.
4. Confirm normal communication.
5. Stop the Vehicle ECU using `Ctrl+C`.
6. Observe the Dashboard and Logger terminals.

The Dashboard can use a timeout mechanism to detect the absence of expected messages.

Example warning:

```text
WARNING: Vehicle ECU Offline
```

The Logger remains active but does not receive new vehicle messages after transmission stops.

### Challenge 6: CAN FD Exploration

**Objective:** Demonstrate CAN FD communication.

CAN FD extends the Classical CAN payload capacity.

| Feature | Classical CAN | CAN FD |
|---|---|---|
| Maximum payload | 8 bytes | 64 bytes |
| Identifier | 11 or 29 bits | 11 or 29 bits |
| Data-rate switching | No | Supported |
| Data length | Up to 8 bytes | Up to 64 bytes |

CAN FD is useful for applications requiring larger payloads and improved data throughput.

Run the receiver first:

```bash
./canfd_receiver
```

In another terminal:

```bash
./canfd_sender
```

Observe the transmitted and received CAN FD messages.

Note: Successful CAN FD operation must be verified from the actual sender and receiver outputs. A virtual interface must support the CAN FD frame format.

### Challenge 7: Basic Diagnostics

**Objective:** Detect loss of vehicle communication.

Run:

```bash
./dashboard_diagnostic
```

The diagnostic application monitors the expected message arrival time.

If no expected message is received within the configured timeout, it reports a communication warning.

Example:

```text
WARNING: Vehicle ECU Offline
```

This demonstrates a basic communication-monitoring mechanism used in automotive systems.

## 9. Troubleshooting

### Error: Address family not supported by protocol

This error can occur when the CAN protocol modules are not loaded or unavailable in the Linux environment.

Load the modules:

```bash
sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan
```

Verify:

```bash
cat /proc/net/protocols | grep CAN
```

Expected output should include:

```text
CAN_RAW
```

Also check:

```bash
lsmod | grep -E 'can|vcan'
```

If the modules are unavailable, check the WSL2 kernel configuration and update WSL from Windows PowerShell:

```powershell
wsl --update
wsl --shutdown
```

Reopen Ubuntu and retry the commands.

### Error: vcan0 already exists

Check:

```bash
ip link show vcan0
```

If it exists:

```bash
sudo ip link set up vcan0
```

Do not create another interface with the same name.

## 10. Results and Learning Outcomes

The project demonstrates the following concepts through separate software-based ECU applications:

- Creation of a virtual CAN interface.
- CAN message transmission using SocketCAN.
- CAN message reception and decoding.
- Simultaneous monitoring by multiple applications.
- CAN identifier-based filtering.
- Logging of CAN traffic into a CSV file.
- Unknown CAN message handling.
- Communication timeout and node failure detection.
- CAN FD sender and receiver implementation.

The project provides an environment for understanding and testing automotive communication concepts before physical CAN hardware is available.

## 11. Conclusion

This project demonstrates how Linux SocketCAN can be used to develop and test automotive CAN communication applications using a software-only network.

By implementing separate Vehicle, Dashboard and Logger ECUs, the project illustrates the basic structure of an automotive communication network.

Additional applications demonstrate message filtering, unknown-message handling, diagnostic monitoring and CAN FD communication.

The implementation provides practical experience with Linux socket programming, CAN frame structures, network monitoring and ECU communication, forming a foundation for future automotive embedded systems development.

---

## 12. GitHub Repository

**Suggested repository name:**

`socketcan-automotive-communication`

**Suggested repository description:**

Software-only CAN and CAN FD communication using Linux SocketCAN and Virtual CAN (vcan0), featuring multiple ECU nodes, real-time dashboard monitoring, traffic logging, message filtering and diagnostic testing.

**Repository topics:**

`socketcan` `can-bus` `can-fd` `automotive` `embedded-systems` `linux` `c-programming` `virtual-can` `ecu`
