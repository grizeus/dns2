#/!bin/bash

BUILD_DIR="build"
cd $BUILD_DIR || exit

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <local_address>"
	exit 1
fi

LOCAL_ADDR=$1

if [ ! -f "config.ini" ]; then
  echo "Creating default config.ini(without black list)"
  cat << EOF > config.ini
Domains =
Upstream = 1.1.1.1
EOF
fi

sudo ./proxy $LOCAL_ADDR
