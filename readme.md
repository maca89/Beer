[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=angrykarl&url=https://github.com/maca89/Beer&title=Beer&language=&tags=github&category=software)

----  COMPILATION  ----
Compiler and VirtualMachine are both written in Visual Studio 2012. To compile the project without VS2012 you need MSBuild and VC++ Redistributable.

You can download VC++ Redistributable here http://www.microsoft.com/en-us/download/details.aspx?id=30679 

MSBuild is contained within .NET framework. It is recommended to use .NET 4.5. 

You can compile both projects with MSBuildu and Build.bat. The executables will be stored in /bin. If you wish to use another version of MSBuild, you must update build.bat.


----  EXECUTION  ----
// Compiles source-file "file.beer" into the class-file "file.class"
// Beware! Beer-compile requires folder /stdlib with standard class definitions.
> beer-compile.exe file.beer

// Runs file.class
> beer-run.exe file.class

// Example of the knapsack
> beer-compile.exe source/knapsack-plain.beer
> beer-run.exe source/knapsack-plain.class

