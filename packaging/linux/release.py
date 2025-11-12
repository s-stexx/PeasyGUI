#!/usr/bin/env python3
import os
import shutil
import subprocess
import tarfile

##########################################################################################
def fp(base_dir, rest_of_path):
  """fullpath function: writing fp(..) instead of os.path.join(..) is shorter"""
  return os.path.join(base_dir, rest_of_path)

##########################################################################################
# Config

TOP_LEVEL_DIR = os.path.abspath(fp(os.path.dirname(__file__), "../.."))
BUILD_DIR = fp(TOP_LEVEL_DIR, "build/MinSizeRel")
DIST_DIR = fp(TOP_LEVEL_DIR, "dist/linux")
STAGING_DIR = fp(DIST_DIR, "PeasyGUI")
QT_DIR = fp(os.path.expanduser("~"), "Qt/6.8.3/gcc_64")
CMAKE_PATH = fp(os.path.expanduser("~"), "Qt/Tools/CMake/bin/cmake")
CMAKE_PREFIX_PATH = fp(QT_DIR, "lib/cmake")

# The needed libs can be figured out with script identify_needs_on_target.py
NEEDED_LIBS = [
  'libQt6Core.so.6.8.3',
  'libQt6DBus.so.6.8.3',
  'libQt6Gui.so.6.8.3',
  'libQt6Network.so.6.8.3',
  'libQt6Widgets.so.6.8.3',
  'libQt6XcbQpa.so.6.8.3',
  'libicudata.so.73.2',
  'libicui18n.so.73.2',
  'libicuuc.so.73.2',
]

REPLACE_EXTENSIONS = {
  '.so.6.8.3': '.so.6',
  '.so.73.2': '.so.73',
}

##########################################################################################
def run_cmd(cmd, cwd=None):
  print("Running:", " ".join(cmd))
  subprocess.check_call(cmd, cwd=cwd)



##########################################################################################
def configure_and_build():
  print(">>> Configuring and building PeasyGUI with CMake ...")

  if os.path.exists(BUILD_DIR):
    shutil.rmtree(BUILD_DIR)
  os.makedirs(BUILD_DIR)

  run_cmd([
    CMAKE_PATH,
    "-S", TOP_LEVEL_DIR,
    "-B", BUILD_DIR,
    "-DCMAKE_BUILD_TYPE=MinSizeRel",
    "-DCMAKE_PREFIX_PATH=" + CMAKE_PREFIX_PATH
  ])

  run_cmd([CMAKE_PATH, "--build", BUILD_DIR, "--parallel"])

##########################################################################################
def build_examples():
  print(">>> Building C greet example ...")

  greet_dir = fp(TOP_LEVEL_DIR, "packaging/examples/3_c_greet")
  greet_build = fp(greet_dir, "build")

  if os.path.exists(greet_build):
    shutil.rmtree(greet_build)
  os.makedirs(greet_build)

  run_cmd([
    CMAKE_PATH,
    "-S", greet_dir,
    "-B", greet_build,
    "-DCMAKE_BUILD_TYPE=MinSizeRel",
    "-DCMAKE_PREFIX_PATH=" + CMAKE_PREFIX_PATH
  ])
  run_cmd([CMAKE_PATH, "--build", greet_build, "--parallel"])

##########################################################################################
def prepare_staging():
  print(">>> Preparing staging directory ...")

  if os.path.exists(STAGING_DIR):
    shutil.rmtree(STAGING_DIR)
  os.makedirs(STAGING_DIR)

  bin_dir = fp(STAGING_DIR, "bin")
  os.makedirs(bin_dir)

  # Copy main binary
  shutil.copy(fp(BUILD_DIR, "PeasyGUI"), bin_dir)

  # Copy licenses N readme
  shutil.copy(fp(TOP_LEVEL_DIR, "third_party_licenses/LICENSE.GPL3"), bin_dir)
  shutil.copy(fp(TOP_LEVEL_DIR, "third_party_licenses/LICENSE.LGPL3"), bin_dir)
  shutil.copy(fp(TOP_LEVEL_DIR, "LICENSE"), bin_dir)
  shutil.copy(fp(TOP_LEVEL_DIR, "README.md"), bin_dir)
  
  # Copy start script
  shutil.copy(fp(TOP_LEVEL_DIR, "packaging/start_PeasyGUI.sh"), STAGING_DIR)

  # Copy examples (but skip .command files)
  examples_src = fp(TOP_LEVEL_DIR, "packaging/examples")
  examples_dst = fp(STAGING_DIR, "examples")
  shutil.copytree(examples_src, examples_dst, ignore=shutil.ignore_patterns("*.command"))

##########################################################################################
def copy_qt_libs():
  print(">>> Copying Qt libraries and plugins needed by PeasyGUI ...")

  folders_n_subfolders = {
    'lib': 'lib',
    'platforms': 'plugins/platforms',
    'imageformats': 'plugins/imageformats',
  }
  
  bin_dir = fp(STAGING_DIR, "bin")
  
  for folder in folders_n_subfolders:
    src_dir = fp(QT_DIR, folders_n_subfolders[folder])
    dst_dir = fp(bin_dir, folder)
    
    os.makedirs(dst_dir, exist_ok=True)
    
    if folder == 'lib':
      # copy only selected
      for lib in NEEDED_LIBS:
        for ext in REPLACE_EXTENSIONS:
          if lib.endswith(ext):
            src_path = fp(src_dir, lib)
            dst_path = fp(dst_dir, lib.replace(ext, REPLACE_EXTENSIONS[ext]))
            shutil.copy(src_path, dst_path)
                        
    else:
      # copy all files
      for file in os.listdir(src_dir):
        src_path = fp(src_dir, file)
        if os.path.isfile(src_path):
          shutil.copy(fp(src_dir, file), dst_dir)

##########################################################################################
def create_tarball():
  print(">>> Creating tarball ...")

  tar_path = fp(DIST_DIR, "PeasyGUI-0000000.tar.gz")
  with tarfile.open(tar_path, "w:gz") as tar:
    tar.add(fp(TOP_LEVEL_DIR, "packaging/linux/Installation_steps.txt"),
        arcname="Installation_steps.txt")
    tar.add(STAGING_DIR, arcname="PeasyGUI")

  print("Created:", tar_path)

##########################################################################################
if __name__ == "__main__":

  configure_and_build()
  build_examples()
  prepare_staging()
  copy_qt_libs()
  create_tarball()
