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

Windows supports a global hotkey, it defaults to ctrl-shift-enter, but can be changed by creating a tracker.ini file in the same directory where tracker.exe is executed.

<pre>
[hotkey]
#A single key (e.g. "x") to bind to, or "space" or "enter"
key=enter
#modifiers applied to the key (defaults to ctrl+shift+enter)
#values: yes or no (lower case)
ctrl=yes
shift=yes
alt=no
</pre>

I was surprised that the windows key modifier was reserved under windows 7, so I couldn't bind to win-T like I do under linux.

Tracked time is output to the file tracker_time.txt in the same directory as the tracker.exe

Linux
-----

Under linux, one must setup a hotkey through their window manager. When tracker is first run, it creates a file ~/tracker.pid. When a second instance is run, it will signal the first instance to display itself.

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
