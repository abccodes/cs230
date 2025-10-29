# 🧪 CS230 Fuzzing Project — AFL++

This project explores coverage-guided fuzzing using AFL++ inside a reproducible Docker environment.  
Everything you need to build, instrument, and run fuzzing experiments is containerized — no system setup required.

---

## 📦 Prerequisites

- Docker installed (https://www.docker.com/products/docker-desktop)
- Git installed
- (Optional) Docker Compose for one-command startup

---

## 🚀 Quick Start

### 1️⃣ Clone the repository
git clone https://github.com/abccodes/cs230.git
cd cs230

### 2️⃣ Build the Docker image
Builds the AFL++ environment defined in Dockerfile.

docker build -t cs230/afl-env .

### 3️⃣ Run the container
Starts an interactive Ubuntu 24.04 shell with your repo mounted at /workspace.

docker run -it --name aflplay \
  --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
  --ulimit core=0 \
  -v "$PWD:/workspace" \
  -w /workspace \
  cs230/afl-env bash

💡 If you see “name already in use” error:
docker rm -f aflplay

---

## 🧰 Inside the Container

You’ll now be inside a full AFL++ environment.  
Basic tools and packages are already installed: clang, make, python3, git, vim, and afl++.

To confirm:
afl-fuzz --version

---

## 🏗️ Building Targets

All source code for fuzz targets lives in the `targets/` folder.

Example:
cd targets
make simpleparse_afl

(See the provided Makefile for more build examples.)

---

## 🔍 Running AFL++

Example fuzzing session:

mkdir -p fuzz/in fuzz/out
echo "seed" > fuzz/in/seed.txt

afl-fuzz -i fuzz/in -o fuzz/out -- ./targets/simpleparse_afl @@

AFL++ will start mutating your inputs and show real-time stats.

---

## 🧩 Using Docker Compose (optional)

If you prefer a one-liner startup, use:

docker compose run --rm afl

# CS230 Fuzzing Project — Docker Setup

This repository uses Docker to provide a reproducible environment for AFL++ fuzzing.  
Follow these instructions to build and run the containerized environment.

---

## Prerequisites

- Docker installed (https://www.docker.com/products/docker-desktop)
- Git installed

---

## Setup Instructions

### 1. Clone the repository
git clone https://github.com/abccodes/cs230.git
cd cs230

### 2. Build the Docker image
docker build -t cs230/afl-env .

### 3. Run the container
docker run -it --name aflplay \
  --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
  --ulimit core=0 \
  -v "$PWD:/workspace" \
  -w /workspace \
  cs230/afl-env bash

If you see an error about the name already being in use, remove the old container:
docker rm -f aflplay

---

## Inside the Container

- Your local project directory is mounted at `/workspace`
- All commands run inside the container will update your local files automatically
- AFL++ and all required tools are already installed

To verify:
afl-fuzz --version

---

## Rebuilding or Cleaning Up

Stop and remove the running container:
docker stop aflplay
docker rm aflplay

Rebuild the image if you change the Dockerfile:
docker build -t cs230/afl-env .

List all containers:
docker ps -a

Remove unused containers:
docker rm <container_id>

---

## Notes

- Always run the container from the root of this project (the folder with the Dockerfile)
- All files under `/workspace` are synced with your local directory
- Do not store large fuzzing outputs in the repo; keep them outside the project folder if needed
This runs the container with all capabilities pre-configured using docker-compose.yml.
