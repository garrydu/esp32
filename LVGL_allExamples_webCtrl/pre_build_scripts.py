import os
import shutil            # Imports the shutil module to use copyfile for file operations
Import("env")            # Imports the 'env' build environment object from PlatformIO

print("Running pre-build script...")  # Prints a message indicating the script is running
# Source and destination paths for the file copy operation
src = "src/lv_conf.h"  # Path to the user configuration file to copy (relative to project root)
# Destination is built dynamically using the current PlatformIO environment (e.g., 'esp32dev')
dst = "{}/{}/lv_conf.h".format(env["PROJECT_LIBDEPS_DIR"], env["PIOENV"])

print("Copying {} to {}".format(src, dst))  # Prints the source and destination paths
shutil.copyfile(src, dst)    # Copies the source file to the destination path

print("Pre-build script completed.")  # Prints a message indicating the script has completed


# Change to the src directory
# os.chdir("src")

split_marker = "1234567\n"
template_file = "src/webpage_src.template"
tmp_file = "src/test.h"
output_file = "src/index_src.h"
input_file = "src/index.html"
# Read up to and including the marker, but do not include lines with the marker itself
with open(template_file, "r", encoding="utf-8") as infile, open(tmp_file, "w", encoding="utf-8") as outfile:
    for line in infile:
        if split_marker.strip() in line:
            break
        if split_marker.strip() not in line:
            outfile.write(line)

# Append all contents of index.html to the temp file
with open(input_file, "r", encoding="utf-8") as htmlfile, open(tmp_file, "a", encoding="utf-8") as outfile:
    shutil.copyfileobj(htmlfile, outfile)

# Append the rest of the template file after the marker
found_marker = False
with open(template_file, "r", encoding="utf-8") as infile, open(tmp_file, "a", encoding="utf-8") as outfile:
    for line in infile:
        if not found_marker:
            if split_marker.strip() in line:
                found_marker = True
            continue
        outfile.write(line)

# Move the temporary file to override index_src.h
os.replace(tmp_file, output_file)