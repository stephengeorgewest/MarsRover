rem necessary for some zip stuff on some systems.
rem regsvr32 /u %windir%\system32\zipfldr.dll 
java  -cp bin;lib/JOGL/jogl.jar;lib/JOGL/gluegen-rt.jar;lib/JInput/;lib/JInput/jinput.jar -Djava.library.path=lib/JInput;lib/JOGL -Xms100m -Xmx150m rover.Main