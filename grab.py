import os

s = ""
for i in os.listdir(os.getcwd()):
    if i.endswith(".cpp"):
        s+=i+" "
print(s)
