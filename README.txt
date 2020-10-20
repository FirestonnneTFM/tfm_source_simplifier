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

example:
--------

JPEX decompiler, after "rename invalid identifiers"

package
{
   public class class_546 extends class_7
   {
      public function class_546(param1:String)
      {
         super(class_134.method_2400(class_143.var_6097),class_134.method_2400(class_168.var_6705));
         var_164.writeUTF(param1);
      }
   }
}

JPEX decompiler, after running this tool first (rename invalid
identifiers not needed)

package
{
   public class PacketOut_31_5 extends PacketOut
   {
      public function PacketOut_31_5(param1:String)
      {
         super(31,5);
         var_0280.writeUTF(param1);
      }
   }
}

license:
--------

zlib is distributed under it's own license

see: https://www.zlib.net/zlib_license.html

The rest of the code: GPLv3

Copyright (C) 2020

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
