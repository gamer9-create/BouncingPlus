import shutil

dst = input("des? ")
if dst == "":
    dst = ".EXE"

shutil.copy("cmake-build-release/BouncingPlus.exe", dst)
shutil.copy("cmake-build-release/glfw3.dll", dst)
shutil.copy("cmake-build-release/raylib.dll", dst)
shutil.copytree("assets", dst+"/assets")
