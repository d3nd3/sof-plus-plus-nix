# sof-plus-plus-nix
a mod that behaves similar to sofplus on windows, applies to linux binary only

## Preparing Build
sudo apt install libcurl4-openssl-dev:i386 libssl-dev:i386 libbsd-dev:i386 libx11-dev:i386 libcrypt1:i386 libminizip-dev:i368

sudo ln -s /usr/lib/i386-linux-gnu/libcrypt.so.1 /usr/lib/i386-linux-gnu/libcrypt.so

## Recommended to use the docker when using USE_PYTHON:=1
It is recommended to use the docker script `dock-build.sh` and `dock-run.sh` to properly install the python library, uses a compiler within a docker!  
Warning: The compiler build docker takes a long time (30min) for first run because it compiles cmake.  

cd into build_python folder.  
run ./grab_files.sh  
copy the lib folder into your sof-runtime folder, the python interpreter will now use PYTHONHOME as .

If your .py addon only is imported by other scripts (used as library), ensure:  
`if __name__ == "builtins":` and indent all the @event decorators. Eg. when exporting a command to a commands.py file  

## Provides
* HTTP Downloads
* Working Server List
* Clipboard to console
* Widescreen resolutions
* Potential python scripting environment for server
* Kill-feed / Server-side ui mods
* Server-side Demos (incomplete)
* Weapon balance modding (incomplete)
* 1.07f compatibility.
* Slide fix (Server)
* Public rcon feature

