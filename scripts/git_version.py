import subprocess
import datetime
import re

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

# --- 1. Update version.h for the firmware ---
header_content = f"""
#ifndef VERSION_H
#define VERSION_H

#define FIRMWARE_VERSION "{version}"
#define BUILD_DATE "{build_date}"

#endif // VERSION_H
"""

# Write the content to a version.h file in the include directory
with open("include/version.h", "w") as header_file:
    header_file.write(header_content)

# --- 2. Update README.md for documentation ---
readme_path = "README.md"
try:
    with open(readme_path, "r") as readme_file:
        readme_content = readme_file.read()

    # The new line to be inserted
    new_version_line = f"**Version:** {version} (Built: {build_date})"

    # Use regex to find the old version line and replace it
    # This regex looks for "**Version:**" followed by any characters until the end of the line.
    updated_readme_content = re.sub(r"(\*\*Version:\*\*.*)", new_version_line, readme_content)

    with open(readme_path, "w") as readme_file:
        readme_file.write(updated_readme_content)
    print(f"Updated {readme_path} with version {version}")
except Exception as e:
    print(f"Warning: Could not update {readme_path}: {e}")