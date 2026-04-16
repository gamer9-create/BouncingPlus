import subprocess
import shutil,os

for i in os.listdir(os.getcwd()):
    if "WEB Dev Build" in i:
        os.remove(i)

def clear(path):
    for p in os.listdir(path):
        f = path+"\\"+p
        if os.path.isfile(f):
            os.remove(f)
        else:
            clear(f)
    os.rmdir(path)

print("Waiting on compiler...")
result = subprocess.run(["web_compile.bat"], capture_output=True, text=True)
print("Compiler Done!")
print(result.stdout)

dst = "build-web"

f = open("version", 'r')
version = int(f.readline())
f.close()
f = open("version",'w')
f.write(str(version+1))
f.close()

print(os.getcwd())

epik_file_name = "Bouncing Plus - Version "+str(version)+" (WEB Dev Build)"

shutil.copytree("../build-web", dst)

shutil.make_archive(epik_file_name, 'zip', dst)

clear(str(os.path.abspath(dst)))
clear(str(os.path.abspath("../build-web")))
