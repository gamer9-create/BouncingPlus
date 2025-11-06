import os

s = ""
for i in os.listdir("../"):
    if i.endswith(".cpp"):
        s+=i+" "
print(s)
