# GitHub Upload Guide: SocketCAN Project

## 📖 Complete Step-by-Step Guide to GitHub

This guide walks you through uploading your SocketCAN project to GitHub from scratch.

---

## Prerequisites

### Before You Start

1. **GitHub Account** - Create one at https://github.com (free)
2. **Git Installed** - Check with: `git --version`
3. **Local Project** - Files ready in `~/socketcan_project/`

### Verify Git Installation

```bash
git --version
# Output should show: git version 2.34.1 (or similar)
```

If Git is not installed:

```bash
sudo apt install git
```

---

## Part 1: GitHub Account Setup

### Step 1.1: Create GitHub Account

1. Go to https://github.com/join
2. Enter username, email, password
3. Verify email address
4. Choose plan (Free is fine)

### Step 1.2: Configure Git Locally

```bash
# Set your GitHub username
git config --global user.name "Your GitHub Username"

# Set your email (same as GitHub account)
git config --global user.email "your.email@example.com"

# Verify configuration
git config --global --list
```

Example:
```bash
git config --global user.name "JohnDoe"
git config --global user.email "john@example.com"
```

---

## Part 2: Initialize Local Git Repository

### Step 2.1: Navigate to Project Directory

```bash
cd ~/socketcan_project
pwd
# Should output: /home/your_username/socketcan_project
```

### Step 2.2: Initialize Git Repository

```bash
git init
# Initialized empty Git repository in /home/.../socketcan_project/.git
```

### Step 2.3: Check Git Status

```bash
git status
# Shows: On branch master
#        No commits yet
#        Untracked files: ...
```

---

## Part 3: Organize Your Project

### Step 3.1: Create Directory Structure

```bash
# Create directories for documentation
mkdir -p docs
mkdir -p screenshots
mkdir -p tests

# Create essential files if missing
touch README.md
touch LICENSE

# List structure
tree -L 2
# Or use: ls -la
```

### Step 3.2: Create .gitignore File

This file tells Git which files NOT to upload:

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

# Generated log files
can_log.csv
*.log

# Editor and IDE files
.vscode/
.idea/
*.swp
*.swo
*~
*.sublime-project
*.sublime-workspace

# OS-specific files
.DS_Store
Thumbs.db
.directory

# Temporary files
*.tmp
*.temp
*.bak

# Build directories
build/
dist/
EOF

echo ".gitignore created"
```

### Step 3.3: Organize Important Files

```bash
# Copy your source code (if not already there)
# cp socketcan_demo.c .

# Move technical report to docs
# cp SocketCAN_Technical_Report.docx docs/

# List files to verify
ls -la
```

Expected structure:
```
socketcan_project/
├── socketcan_demo.c
├── README.md
├── LICENSE
├── .gitignore
├── docs/
│   ├── SocketCAN_Technical_Report.docx
│   ├── SETUP.md
│   └── CHALLENGES.md
└── screenshots/
    ├── dashboard.png
    ├── logger.png
    └── candump.png
```

---

## Part 4: Create Initial Commits

### Step 4.1: Add All Files

```bash
# Add all files except those in .gitignore
git add .

# Check what will be committed
git status

# Should show: Changes to be committed:
#                new file:   .gitignore
#                new file:   LICENSE
#                new file:   README.md
#                etc.
```

### Step 4.2: Create First Commit

```bash
git commit -m "Initial commit: SocketCAN software-only CAN implementation

- Comprehensive single-file C implementation
- Vehicle ECU (transmitter)
- Dashboard ECU (receiver with diagnostics)
- Logger ECU (traffic recording)
- CAN message filtering support
- CAN FD capability (64-byte payloads)
- Timeout-based offline detection
- Complete documentation and testing guide
- Ready for development and production use"
```

### Step 4.3: Verify Commit

```bash
git log --oneline
# Should show your commit

git status
# Should show: On branch master
#              nothing to commit, working tree clean
```

---

## Part 5: Create GitHub Repository

### Step 5.1: Create New Repository on GitHub

1. **Log in to GitHub** at https://github.com
2. Click the **+** icon (top-right corner)
3. Select **New repository**

### Step 5.2: Fill Repository Details

| Field | Value |
|-------|-------|
| Repository name | `socketcan-demo` |
| Description | `Software-only CAN/CAN FD communication using Linux SocketCAN` |
| Visibility | Public (for sharing) or Private (for yourself) |
| Initialize README | **NO** (you already have one) |
| Add .gitignore | **NO** (you already have one) |
| Choose a license | MIT (or as preferred) |

### Step 5.3: Create Repository

Click **Create repository**

You will see:
```
…or create a new repository on the command line
git remote add origin https://github.com/USERNAME/socketcan-demo.git
git branch -M main
git push -u origin main
```

**COPY THIS INFORMATION** - You'll use it next.

---

## Part 6: Connect Local Repository to GitHub

### Step 6.1: Add Remote Origin

Replace `USERNAME` with your GitHub username:

```bash
git remote add origin https://github.com/USERNAME/socketcan-demo.git
```

Example:
```bash
git remote add origin https://github.com/JohnDoe/socketcan-demo.git
```

### Step 6.2: Verify Remote

```bash
git remote -v

# Should show:
# origin  https://github.com/USERNAME/socketcan-demo.git (fetch)
# origin  https://github.com/USERNAME/socketcan-demo.git (push)
```

### Step 6.3: Rename Branch to Main

```bash
git branch -M main

git branch
# Should show: * main
```

---

## Part 7: Push to GitHub

### Step 7.1: Push Your Code

```bash
git push -u origin main

# First push should show:
# Enumerating objects: 10, done.
# Counting objects: 100% (10/10), done.
# ...
# To https://github.com/USERNAME/socketcan-demo.git
#  * [new branch]      main -> main
# Branch 'main' set up to track remote branch 'main' from 'origin'.
```

### Step 7.2: Handle Authentication

**If prompted for password:**

GitHub no longer accepts passwords. Use **Personal Access Token (PAT)**:

1. Go to https://github.com/settings/tokens
2. Click **Generate new token**
3. Give it a name: "Git CLI"
4. Check: `repo`, `write:packages`, `read:packages`
5. Click **Generate token**
6. **COPY THE TOKEN** (you won't see it again)

```bash
# When Git asks for password, PASTE THE TOKEN
# Not your password, but the token you just created
```

**Alternatively, use SSH:**

```bash
# Generate SSH key (if you don't have one)
ssh-keygen -t ed25519 -C "your.email@github.com"

# Press Enter for all prompts (use default location)
# This creates ~/.ssh/id_ed25519 and ~/.ssh/id_ed25519.pub

# Add public key to GitHub:
# 1. Go to https://github.com/settings/keys
# 2. Click "New SSH key"
# 3. Paste contents of: cat ~/.ssh/id_ed25519.pub
# 4. Click "Add SSH key"

# Update remote to use SSH:
git remote set-url origin git@github.com:USERNAME/socketcan-demo.git

# Try push again:
git push -u origin main
```

### Step 7.3: Verify Upload

Visit: `https://github.com/USERNAME/socketcan-demo`

You should see:
- ✅ All your files listed
- ✅ README.md displayed beautifully
- ✅ Code syntax highlighting
- ✅ Commit history visible

---

## Part 8: Make Updates and Changes

### Workflow for Making Changes

**After modifying files:**

```bash
# Check what changed
git status

# Stage changes
git add socketcan_demo.c
# Or add everything:
git add .

# Commit with message
git commit -m "Fix: improve error handling in signal encoding"

# Push to GitHub
git push
```

### Example Workflow

```bash
# Edit your code
nano socketcan_demo.c

# Check changes
git status
# Output: modified:   socketcan_demo.c

# Stage the change
git add socketcan_demo.c

# Commit
git commit -m "Add support for extended CAN identifiers"

# Push
git push

# Verify on GitHub
# Visit your repository and refresh
```

---

## Part 9: Branching (Optional but Recommended)

### Create Feature Branches

```bash
# Create a new branch
git checkout -b feature/can-fd-support

# Make changes, commit
git add .
git commit -m "Implement CAN FD frame handling"

# Push branch to GitHub
git push -u origin feature/can-fd-support

# Create Pull Request on GitHub (UI)
# - Go to your repository
# - GitHub will show "Compare & pull request"
# - Click it, add description, merge to main
```

### Merge Branch Back to Main

```bash
# Switch to main
git checkout main

# Merge the feature branch
git merge feature/can-fd-support

# Push merged result
git push

# Delete feature branch (optional cleanup)
git branch -d feature/can-fd-support
git push origin --delete feature/can-fd-support
```

---

## Part 10: Helpful GitHub Features

### Add GitHub Actions (CI/CD - Optional)

Create `.github/workflows/test.yml`:

```bash
mkdir -p .github/workflows

cat > .github/workflows/test.yml << 'EOF'
name: Build Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v2
      - name: Compile
        run: gcc -Wall -Wextra -O2 socketcan_demo.c -o socketcan_demo -lm
EOF

git add .github/
git commit -m "Add GitHub Actions CI/CD"
git push
```

Now GitHub automatically tests your code on every push!

### Add Issue Templates

Create `.github/ISSUE_TEMPLATE/bug_report.md`:

```bash
mkdir -p .github/ISSUE_TEMPLATE

cat > .github/ISSUE_TEMPLATE/bug_report.md << 'EOF'
---
name: Bug report
about: Report a bug
---

## Description
[Describe the bug]

## Steps to Reproduce
1. [First step]
2. [Second step]

## Expected behavior
[What should happen]

## Actual behavior
[What actually happens]

## Environment
- OS: [Ubuntu version]
- GCC version: [output of gcc --version]

## Additional context
[Any additional info]
EOF

git add .github/
git commit -m "Add bug report template"
git push
```

---

## Troubleshooting

### Problem: "fatal: not a git repository"

**Solution:**
```bash
cd ~/socketcan_project
git init
```

### Problem: "failed to push some refs to 'origin'"

**Solution:**
```bash
# Pull latest changes first
git pull origin main

# Then push
git push origin main
```

### Problem: "Authentication failed" or "Permission denied"

**Solution 1: Use Personal Access Token**
```bash
# Generate token at: https://github.com/settings/tokens
# Use token as password when prompted
```

**Solution 2: Use SSH**
```bash
# Setup SSH key
ssh-keygen -t ed25519 -C "email@example.com"

# Add to GitHub: https://github.com/settings/keys

# Change remote to SSH
git remote set-url origin git@github.com:USERNAME/socketcan-demo.git

# Try again
git push
```

### Problem: "rejected (fetch first)"

**Solution:**
```bash
git fetch origin
git merge origin/main
git push origin main
```

### Problem: "nothing to commit, working tree clean" but changes not on GitHub

**Solution:**
```bash
# Make sure you've added and committed
git status  # Check status

# If nothing staged:
git add .
git commit -m "Your message"
git push
```

---

## Quick Reference

### Most Used Git Commands

```bash
# Clone someone else's repo
git clone https://github.com/someone/project.git

# Check status
git status

# Add changes (all files)
git add .

# Add specific file
git add filename.c

# Commit changes
git commit -m "Your message"

# Push to GitHub
git push

# Pull latest changes
git pull

# View commit history
git log --oneline

# Create new branch
git checkout -b branch-name

# Switch branches
git checkout main

# Delete branch
git branch -d branch-name

# Merge branches
git merge branch-name

# View remote info
git remote -v

# Change remote URL
git remote set-url origin https://new-url
```

---

## Final Checklist

Before considering your project "complete" on GitHub:

- [ ] Repository created on GitHub
- [ ] Local repo connected with `git remote add origin`
- [ ] All files pushed to GitHub
- [ ] README.md displays correctly
- [ ] .gitignore working (compiled binaries not uploaded)
- [ ] LICENSE file present
- [ ] Commit history visible
- [ ] Can be cloned by others: `git clone https://github.com/USERNAME/socketcan-demo.git`

---

## Advanced: GitHub Pages (Optional)

Make your project documentation visible as a website:

1. Go to repository Settings
2. Scroll to "GitHub Pages"
3. Select "main" branch as source
4. Choose theme
5. Your README.md becomes a website!

Visit: `https://USERNAME.github.io/socketcan-demo/`

---

## Example Complete Session

```bash
# 1. Setup
cd ~/socketcan_project
git init
git config user.name "MyName"
git config user.email "my@email.com"

# 2. Create .gitignore and README
cat > .gitignore << 'EOF'
socketcan_demo
*.o
can_log.csv
.DS_Store
EOF

# 3. Commit
git add .
git commit -m "Initial commit: SocketCAN project"

# 4. Create GitHub repo (via web interface)
# Go to https://github.com/new
# Name: socketcan-demo
# Click Create Repository

# 5. Connect to GitHub
git remote add origin https://github.com/USERNAME/socketcan-demo.git
git branch -M main

# 6. Push
git push -u origin main

# 7. Verify
# Visit https://github.com/USERNAME/socketcan-demo

# 8. Make changes later
# Edit code...
git add socketcan_demo.c
git commit -m "Add feature X"
git push
```

---

## Getting Help

**GitHub Help:** https://docs.github.com
**Git Documentation:** https://git-scm.com/doc
**Markdown Guide:** https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet

---

## Summary

You now have:
1. ✅ A local Git repository
2. ✅ A GitHub repository
3. ✅ Your code uploaded and visible
4. ✅ Version control and history
5. ✅ Ability to collaborate
6. ✅ Backup in the cloud
7. ✅ Portfolio piece for job applications

**Congratulations! Your SocketCAN project is now on GitHub! 🎉**

