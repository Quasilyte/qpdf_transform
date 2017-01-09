#!/bin/bash

qpdf_path1='/usr/local/include/qpdf/'
qpdf_path2='/usr/share/include/qpdf/'

if [ -d $qpdf_path1 ] || [ -d $qpdf_path2 ]; then
    echo 'ok: found qpdf headers'
else
    echo "error: qpdf headers not found (searched in $qpdf_path1 and $qpdf_path2)"
fi

