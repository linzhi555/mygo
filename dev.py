import build
import subprocess
import os
if __name__ == "__main__":
    build.build()
    args = [os.path.join("./",build.EXE)]
    subprocess.run(args)
