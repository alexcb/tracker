Dead simple time tracker
========================
I wanted an easy to use time tracker that:

* didn't require a mouse, or excessive keystrokes
* took less than a second to load
* detected idle activity
* could export data in a plain text format

Surprisingly I had to write my own -- here is my first initial (code) release.

If anyone finds this useful please send me (Alex Couture-Beil) an email: alex+github@mofo.ca 

tracker can be compiled for both Windows and Linux.

Windows Details
---------------

Build instructions:
-requires: QT v5
-cmake

1: Create a solution file with the following steps
Edit the following variables in windows_generate_solution.bat:
-update PATH to include the location of cmake.exe, and vcexpress.exe
-update QTROOT to point to the root qt directory
Then run the bat script

2: compiling a release version:
-open build\tracker.sln
-select "Release" from build drop down
-build

If running the program through visual studio, you will need to update the path to include the DLL locations:
tracker -> right click -> properties -> Debugging.
Set Environment to include:

    PATH=C:\qt\Qt5.0.2\5.0.2\msvc2010\bin;%PATH%

3: packaging:
Edit the following variables in windows_package_release.bat:
-update QTROOT to point to the root qt directory
Then run the bat script





Linux
-----

Under linux, one must setup a hotkey through the window manager. When tracker is first run, it creates a file ~/tracker.pid. When a second instance is run, it will signal the first instance to display itself.

Under xfce, one can add a custom keyboard shortcut under: settings -> keyboard -> application shortcuts

tracked time is output to the file ~/.tracker_time


Tracker Time Output
-------------------

tracked time is logged to plain text file in the form:

<timestamp> <task name>

where timestamp is a UTC epoch timestamp in seconds, and task name is any string with or without whitespace.


License
=======
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Build Instructions
==================

I hope to offer pre-built binaries one day, but for now it's just code.

required libraries (for both windows, linux):

* wxWidgets **2.9.4 -- not the 2.8 branch that most distros package**

required tools:

* cmake

###Linux

additional required libraries: 

* libxss-dev

<pre>
cmake -G "Unix Makefiles"
make
</pre>

###Windows

additional required libraries:

* Boost (headers only)

<pre>
cmake -G "Visual Studio 10"
open tracker.sln (tested with VC++ 2010 express)
</pre>
