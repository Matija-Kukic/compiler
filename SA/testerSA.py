import os
import subprocess


sadir = input("Unesite ime direktorija gdje vam je kod: ")

cwd = os.getcwd()
tests = cwd + "/integration"
os.chdir(tests)
items = os.listdir()
os.chdir(cwd)
ts, cr = 0, 0
for it in items:
    ts += 1
    print(f"\033[33mTESTING {it}\033[0m")
    subprocess.run(["cp", "integration/" + it + "/test.san", sadir])
    subprocess.run(["cp", "integration/" + it + "/test.in", sadir + "/analizator"])
    subprocess.run(["cp", "integration/" + it + "/test.out", sadir + "/analizator"])
    nd = cwd + "/" + sadir
    os.chdir(nd)
    file = open("test.san", "r")
    subprocess.run(["./generator"], stdin=file, text=True)
    file.close()
    nd2 = nd + "/analizator"
    os.chdir(nd2)
    file = open("test.in", "r")
    file2 = open("izlaz.txt", "w")
    subprocess.run(
        ["./analizator"], stdin=file, stdout=file2, stderr=subprocess.DEVNULL
    )
    file.close()
    file2.close()
    out = subprocess.run(
        ["diff", "izlaz.txt", "test.out"], text=True, capture_output=True
    )
    if out.stdout == "":
        print(f"\033[32mTEST {it} PASSED\033[0m")
        cr += 1
    else:
        print(f"\033[31mTEST {it} FAILED\033[0m")
    subprocess.run(["rm", "izlaz.txt"])
    subprocess.run(["rm", "test.out"])
    subprocess.run(["rm", "test.in"])
    os.chdir(nd)
    subprocess.run(["rm", "test.san"])
    os.chdir(cwd)


print(f"\033[34m{cr}/{ts} TESTS PASSED\033[0m")
