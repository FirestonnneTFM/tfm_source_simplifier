compile commands:
-----------------

first extract zlib from the zip folder, so that the current directory
has a zlib-x.x.x subdirectory

$ mkdir build
$ cd build
$ cmake ..
( note: running just `make` will probably fail because the zlib tests
don't compile )
$ make tfm_src_simple

run command:
------------

$ ./tfm_src_simple input_file.swf output_file.swf

features:
---------

* removes the layers of indirection regarding the static classes full of
constants

* generally renames the invalid class names in a way that makes their
inheritance more clear

* finds classes that inherit from the input or output packet base class

license note:
-------------

All rights reserved.

Do not distribute.

(obviously this doesn't apply to zlib,
see: https://www.zlib.net/zlib_license.html )
