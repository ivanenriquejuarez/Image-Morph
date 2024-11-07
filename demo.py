import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--run",action="store_true")
args = parser.parse_args()

if args.run:
    subprocess.call(["gcc", "main.c"])
    subprocess.call("./a.out")