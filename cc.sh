#!/bin/bash
set -e
echo "Putting all together..."
make
echo "WOW, this shit compiled"
bin/main "$@"
echo "You witnessed a real miracle..."
