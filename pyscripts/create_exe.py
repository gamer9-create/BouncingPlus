import shutil,os

for i in os.listdir(os.getcwd()):
    if ".zip" in i:
        os.remove(i)

def clear(path):
    for p in os.listdir(path):
        f = path+"\\"+p
        if os.path.isfile(f):
            os.remove(f)
        else:
            clear(f)
    os.rmdir(path)
    

dst = ".EXE"

f = open("version", 'r')
version = int(f.readline())
f.close()
f = open("version",'w')
f.write(str(version+1))
f.close()

os.mkdir(dst)

shutil.copy("../cmake-build-release/BouncingPlus.exe", dst)
for file in os.listdir("../cmake-build-release"):
    if file.endswith(".dll"):
        shutil.copy("../cmake-build-release/"+file, dst)
shutil.copytree("../assets", dst+"/assets")

shutil.make_archive("BouncingPlusVersion"+str(version), 'zip', dst)

path = "BouncingPlusVersion"+str(version)+".zip"
#command = f"file2clip.exe \"{path}\""
#os.system(command)

clear(str(os.path.abspath(dst)))
