#!/bin/bash

# backs up all files in current directory modified within last 24 hours
# +in a "tarball" (tarred and gzipped file).

BACKUP_FILES=backup-$(date +%m-%d-%Y)
# Embeds date in backup filename.

archive=${1:-$BACKUP_FILES}
# if no backup-archive filename specified on command line,
# +it will default to "backup-MM-DD-YYYY.tar.gz

tar cvf - `find . -mtime -1 -type -f -print` > $archive.tar
gzip $archive.tar
echo "Directory $PWD backed up in the archive file \"$archive.tar.gz\"."

# too many files or any filenames contain blank characters
#   find . -mtime -1 -type f -print0 | xargs -0 tar rvf "$archive.tar"
#      using the GNU version of "find".

#   find . -mtime -1 -type f -exec tar rvf "$archive.tar" '{}' \;
#         portable to other UNIX flavors, but much slower.
# -------------------------------------------------------------------
exit 0
