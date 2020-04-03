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
   public class packet_out_0078 extends PacketOut
   {
      public function packet_out_0078(param1:String)
      {
         super(31,5);
         var_0280.writeUTF(param1);
      }
   }
}


license note:
-------------

All rights reserved.

Do not distribute.

(obviously this doesn't apply to zlib,
see: https://www.zlib.net/zlib_license.html )
