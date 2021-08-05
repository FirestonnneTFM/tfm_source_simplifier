# What does this do?

* Removes the layers of indirection regarding the static classes full of constants

* Generally renames the invalid class names in a way that makes their inheritance more clear

* Finds classes that inherit from the input or output packet base class

# Compile Commands

First extract zlib from the zip folder, so that the current directory has a zlib-x.x.x subdirectory

```
$ mkdir build
$ cd build
$ cmake ..
$ make tfm_src_simple
```
(Note: running just `make` will try to compile zlib tests which may fail preventing you from compiling. That's why I recommend typing the full target name above)

# Run Command

```
$ ./tfm_src_simple input_file.swf output_file.swf
```

# Example

Before: decompiled with JPEX decompiler, after "rename invalid identifiers"

```actionscript
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
```

After: decompiled with JPEX decompiler after running this tool first (rename invalid identifiers does not need to be run)

```actionscript
package
{
   public class PacketOut_31_5 extends PacketOut
   {
      public function PacketOut_31_5(param1:String)
      {
         super(31,5);
         buffer.writeUTF(param1);
      }
   }
}
```

# License

zlib is distributed under it's own license

see: https://www.zlib.net/zlib_license.html

The rest of the code is under GPLv3

Copyright (C) 2020 - 2021

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
