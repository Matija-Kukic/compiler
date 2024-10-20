import os
import subprocess

cwd = os.getcwd()
tests = cwd + "/integration"
print(tests)
os.chdir(tests)

items = os.listdir()
types = set()
for item in items:
    print(item)
    a = item.split(".")
    types.add(a[0])
print(types)
os.chdir(cwd)
tn = 0
for it in types:
    lang = it + ".lan"
    iN = it + ".in"
    out = it + ".out"
    # print(lang, iN, out)
    subprocess.run(["cp", "integration/" + lang, "lexicAnalyzer/"])
    subprocess.run(["cp", "integration/" + iN, "lexicAnalyzer/analizator"])
    subprocess.run(["cp", "integration/" + out, "lexicAnalyzer/analizator/correct.txt"])
    nd = cwd + "/lexicAnalyzer"
    os.chdir(nd)
    subprocess.run(["g++", "generator.cpp", "-o", "generator"])
    file = open(lang, "r")
    subprocess.run(["./generator"], stdin=file, text=True)
    file.close()
    nd2 = nd + "/analizator"
    os.chdir(nd2)
    file = open(iN, "r")
    file2 = open("izlaz.txt", "w")
    subprocess.run(["g++", "analizator.cpp", "automat.cpp", "-o", "analizator"])
    subprocess.run(
        ["./analizator"], stdin=file, stdout=file2, stderr=subprocess.DEVNULL
    )
    file.close()
    file2.close()
    out = subprocess.run(
        ["diff", "izlaz.txt", "correct.txt"], text=True, capture_output=True
    )
    if out.stdout == "":
        print(f"TEST {it} PASSED")
    else:
        print("FAILL", out.stdout)

        break
    subprocess.run(["rm", "izlaz.txt"])
    subprocess.run(["rm", "correct.txt"])
    subprocess.run(["rm", iN])
    os.chdir(nd)
    subprocess.run(["rm", lang])
    os.chdir(cwd)
