#!/bin/bash

cd "${0%/*}/../Resources/"
../MacOS/atd >~/Library/Logs/atd.log 2>&1
