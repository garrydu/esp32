import shutil            # Imports the shutil module to use copyfile for file operations
Import("env")            # Imports the 'env' build environment object from PlatformIO

print("Running pre-build script...")  # Prints a message indicating the script is running
# Source and destination paths for the file copy operation
src = "src/GUIslice_config.h"  # Path to the user configuration file to copy (relative to project root)
# Destination is built dynamically using the current PlatformIO environment (e.g., 'esp32dev')
dst = "{}/{}/GUIslice/src/GUIslice_config.h".format(env["PROJECT_LIBDEPS_DIR"], env["PIOENV"])

print("Copying {} to {}".format(src, dst))  # Prints the source and destination paths
shutil.copyfile(src, dst)    # Copies the source file to the destination path

print("Pre-build script completed.")  # Prints a message indicating the script has completed