import subprocess
CC="g++"
FLAGS=["-o1","-std=c++17","-g", "-fno-exceptions"]
SRC="main.cpp"
EXE="myc"

def build():
    args = []
    args.append(CC)
    args.extend(FLAGS)
    args.extend([SRC,"-o",EXE])
    subprocess.run(args=args)
if __name__ == "__main__":
    build()
