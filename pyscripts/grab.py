import os

def cool(dir1):
    s = ""
    if "cmake" in dir1:
        return s
    for i in os.listdir(dir1):
        i = str(i)
        p = os.path.join(dir1, i)
        if os.path.isdir(p):
            s += cool(p)
        else:
            if i.endswith(".cpp") or i.endswith(".h"):
                s += p + " "
    return s
os.chdir(os.path.join(os.getcwd(), "..\\"))
s = cool(".\\")
f = s.split(" ")
g = ""
for p in f:
    g += (str(p).replace("\\","/")+ " ")[2:]
print(g)
