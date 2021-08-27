FirstFit.c
	A memory allocation method in which the first available block of either the same size or larger of the new process is used from memory
	in order to keep memory use to a minimum, and any extra space is turned into a new block of available memory.
	
BestFit.c:
	A memory allocation method in which the available block that is the closest size to the new process is used from memory 
	in order to keep memory use to a mimimum, and any extra space is turned into a new block of available memory.
	
WorstFit.c:
	A memory allocation method in which the largest avaiable block is used in order to keep memory use to a minimum,
	with the extra space being turned into a new block of available memory.

COMPILING:
	Compile each of the files by entering the following ...
		make *****
			where **** may be FirstFit, WorstFit or BestFit.

EXECUTING:
	Execute each of the files by entering the following ...
		./**** first-names.txt
		./**** middle-names.txt
			where **** may be FirstFit, WorstFit or BestFit.
			
OUTPUT:
	After each algorithm is compiled and executed, the results will be printed to the console.
	As well as this, each output for either first-name or middle name will create a corresponding
	TXT file with the results in the format style ...
		Middle name : *****Middle.txt
		First name :  *****FirstN.txt
			where **** may be FirstFit, WorstFit or BestFit.
	
	Samples of these outputs have been included and will be referenced in the report, however upon execution these 
	will be overritten with new files.