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
