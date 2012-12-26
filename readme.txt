----  KOMPILACE  ----
Kompilátor i VirtualMachine jsou psány ve Visual Studiu 2012, tzn vyžadují pro kompilaci MSBuild a pro spuštění Visual C++ Redistributable.

Stažení VC++ Redistributable například zde http://www.microsoft.com/en-us/download/details.aspx?id=30679

MSBuild je obsažen v .NET frameworku. Doporučujeme použít .NET v4. 

Build.bat zkompiluje pomocí MSBuildu oba projekty. Spustitelné soubory umístí do složky bin. Pokud chcete použít jinou verzi MSBuildu, upravte příslušná místa v build.bat.


----  SPUŠTĚNÍ  ----
// Zkompiluje soubor file.beer do file.class
// Pozor, beer-compile vyžaduje složku /stdlib s definicemi vestavěných tříd
> beer-compile.exe file.beer

// Spustí file.class
> beer-run.exe file.class

// Příklad spuštění batohu
> beer-compile.exe source/knapsack.beer
> beer-run.exe source/knapsack.class in1.txt out1.txt

// Vstupní soubor
kapacita_batohu pocet_veci vec1 vec2 ...

// Výstupní soubor
Knapsack capacity: kapacita_batohu
Items: vec1 vec2 ...
Solution: not found nebo seznam věcí, které jsou v řešení
Time: doba_vypoctu 