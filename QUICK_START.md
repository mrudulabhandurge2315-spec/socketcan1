# SocketCAN Quick Start Guide

**Get up and running in 5 minutes!**

---

## 1️⃣ Install Dependencies (1 minute)

```bash
sudo apt update
sudo apt install build-essential can-utils iproute2 git
```

---

## 2️⃣ Setup Virtual CAN (1 minute)

```bash
# Load vcan module
sudo modprobe vcan

# Create virtual CAN interface
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

# Verify
ip link show vcan0
# Should show: vcan0: <NOARP,UP,LOWER_UP>
```

---

## 3️⃣ Compile Project (1 minute)

```bash
# Create project directory
mkdir -p ~/socketcan_project
cd ~/socketcan_project

# Copy socketcan_demo.c here (or download it)

# Compile
gcc -Wall -Wextra -O2 socketcan_demo.c -o socketcan_demo -lm

# Verify
ls -l socketcan_demo
# Should exist and be executable
```

---

## 4️⃣ Run the System (2 minutes)

**Open 4 Terminal Windows:**

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

**Terminal 3 - Vehicle:**
```bash
cd ~/socketcan_project
./socketcan_demo vehicle
```

**Terminal 4 - Monitor:**
```bash
candump vcan0
```

---

## ✅ Expected Output

### Dashboard Terminal
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

### Logger Terminal
```
Logger ECU started.
Logging to can_log.csv

[1750000000.123456] ID=0x100 DLC=2 Data=38 00
[1750000000.123500] ID=0x101 DLC=2 Data=DC 03
[1750000000.123530] ID=0x102 DLC=2 Data=49 00
```

### Monitor Terminal
```
vcan0  100   [2]  38 00
vcan0  101   [2]  DC 03
vcan0  102   [2]  49 00
```

---

## 🧪 Quick Tests

### Test 1: Message Filtering

```bash
# Dashboard - Speed only
./socketcan_demo dashboard speed

# Dashboard - RPM only
./socketcan_demo dashboard rpm

# Dashboard - All
./socketcan_demo dashboard all
```

### Test 2: Unknown Message

Terminal 1:
```bash
./socketcan_demo unknown
```

Terminal 2:
```bash
candump vcan0
# Will show 0x200 message
```

### Test 3: Node Failure

Run all three nodes, then stop Vehicle ECU (Ctrl+C).
Wait 2 seconds - Dashboard shows:
```
WARNING: Vehicle ECU Offline
```

### Test 4: CAN FD

Terminal 1:
```bash
./socketcan_demo canfd_rx
```

Terminal 2:
```bash
./socketcan_demo canfd_tx
```

---

## 📊 View Log File

```bash
# Show all logs
cat can_log.csv

# Show first 10 entries
head -10 can_log.csv

# Show last 10 entries
tail -10 can_log.csv

# Count messages
wc -l can_log.csv

# Search for speed messages only
grep "^.*,0x100," can_log.csv
```

---

## 🛠️ Common Issues

### vcan0 doesn't exist
```bash
sudo ip link show vcan0
# ERROR: unknown device
# Solution: Run setup again
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

### Permission denied
```bash
sudo chmod +x socketcan_demo
./socketcan_demo vehicle
```

### Compilation failed
```bash
# Make sure gcc is installed
gcc --version

# Recompile with verbose output
gcc -Wall -Wextra -v socketcan_demo.c -o socketcan_demo -lm
```

### No module named vcan
```bash
# Make sure kernel module is loaded
lsmod | grep vcan

# If not present:
sudo modprobe vcan
lsmod | grep vcan
```

---

## 🎯 Performance Testing

### Test different transmission rates:

Edit `socketcan_demo.c` - find `usleep()` and change value:

```c
// Default (500ms - 2 msg/sec)
usleep(500000);

// Experiment 1: Slow (1 second)
usleep(1000000);

// Experiment 2: Fast (100ms - 10 msg/sec)
usleep(100000);

// Experiment 3: Very Fast (10ms - 100 msg/sec)
usleep(10000);
```

Recompile and observe:
- Dashboard responsiveness
- Log file growth
- CPU usage

---

## 📊 CAN Message Format

All use **little-endian** encoding:

| Message | CAN ID | Data Bytes | Example |
|---------|--------|-----------|---------|
| Speed | 0x100 | [LSB, MSB] | 65 km/h = [0x41, 0x00] |
| RPM | 0x101 | [LSB, MSB] | 2450 rpm = [0x92, 0x09] |
| Temperature | 0x102 | [LSB, MSB] | 88°C = [0x58, 0x00] |

---

## 📤 Upload to GitHub

### Quick GitHub Upload:

```bash
# Initialize git
cd ~/socketcan_project
git init
git config user.name "Your Name"
git config user.email "your@email.com"

# Add files
git add .
git commit -m "SocketCAN implementation"

# Create repository on GitHub (web interface)
# Then connect and push:
git remote add origin https://github.com/USERNAME/socketcan-demo.git
git branch -M main
git push -u origin main
```

**See GITHUB_SETUP.md for detailed instructions**

---

## 🎓 Next Steps

1. ✅ **Understand SocketCAN** - Read README.md
2. ✅ **Run Demonstrations** - Follow testing section
3. ✅ **Modify Code** - Try adding new features
4. ✅ **Upload to GitHub** - Share your project
5. ✅ **Write Report** - Use Technical Report template

---

## 📚 Key Concepts

- **SocketCAN**: Linux standard for CAN networking
- **vcan0**: Virtual CAN interface (no hardware needed)
- **CAN ID**: Message identifier (0x100, 0x101, etc.)
- **DLC**: Data Length Code (bytes in message)
- **Message Filtering**: Receive only specific messages
- **Timeout Detection**: Detect node offline
- **CAN FD**: Extended payloads (up to 64 bytes)

---

## ⚡ Pro Tips

1. **Monitor all traffic while developing:**
   ```bash
   candump vcan0
   ```

2. **Send test messages:**
   ```bash
   cansend vcan0 100#4100  # Speed 65
   cansend vcan0 101#9209  # RPM 2450
   cansend vcan0 102#5800  # Temp 88
   ```

3. **Save traffic to file:**
   ```bash
   candump vcan0 -l socketcan.log
   ```

4. **Generate random traffic:**
   ```bash
   cangen vcan0
   ```

5. **Check system resources:**
   ```bash
   top          # Press 'q' to quit
   ps aux | grep socketcan_demo
   ```

---

## ✅ Verification Checklist

- [ ] vcan0 is UP and running
- [ ] socketcan_demo compiles without errors
- [ ] Vehicle ECU transmits messages
- [ ] Dashboard receives and displays values
- [ ] Logger records to CSV file
- [ ] All three run simultaneously
- [ ] Node failure is detected
- [ ] CAN FD frames work
- [ ] Traffic monitoring with candump works

---

## 🚀 You're Ready!

Your SocketCAN system is now operational. Enjoy exploring!

**Questions?** Check README.md for comprehensive documentation.

