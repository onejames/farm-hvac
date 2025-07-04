import subprocess
import datetime

Import("env")

# Get the version from the git tag
try:
    # Using git describe to get a version string. Example: "v0.3.0-5-g1234567"
    # --tags: Use tags
    # --always: Fallback to commit hash if no tag
    # --dirty: Append -dirty if the working directory has modifications
    version = subprocess.check_output(["git", "describe", "--tags", "--always", "--dirty"]).strip().decode('utf-8')
except:
    version = "unknown"

# Get the current date and time
build_date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# Define the content of the header file
header_content = f"""
#ifndef VERSION_H
#define VERSION_H

#define FIRMWARE_VERSION "{version}"
#define BUILD_DATE "{build_date}"

#endif // VERSION_H
"""

# Write the content to a version.h file in the include directory
with open("include/version.h", "w") as f:
    f.write(header_content)