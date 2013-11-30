/* kernel.c
   A simple Kernel that displays a message.
   Ashwin Chandrasekhar and Preethi Rajagopalan
*/

void printString(char* String);             // Prints a string in the screen
void readString(char* String);              // Reads a string from the input in the keyboard
void readSector(char* buffer, int sector);  // Reads a sector of the image in the disk.
void handleInterrupt21(int ax, int bx, int cx, int dx); // For the execution of interrupt21
int mod(int a, int b);       // For Calculating mod function
void readFile(char* filename, char* buffer);
int ifFound(char* dir, int index, char* filename);
void executeProgram(char* name, int segment);
void addFile(char* dir, int index, char* filename);
void terminate();
char line[80];
char buffer[13312];
int output, j;

//<------------------------------------  Main Function-------------------------------------->

int main() {
	//printString("Hello World\r\n\0");   // related to step 1    
	//printString("Enter a line: \0");
	//printString("\r\n\0");    
	//readString(line);                   // related to step 2
	//printString("\r\n\0");
	//printString(line);                  // related to step 2
	//printString("\r\n\0");
	//readSector(buffer, 30);             // related to step 3
	//printString("\r\n\0");
	//printString(buffer);                // related to step 3
	//printString("\r\n\0");
	//makeInterrupt21();                  // related to step 4
	//interrupt(0x21, 1, line, 0, 0);     // related to step 5
	//printString("\r\n\0");
	//interrupt(0x21, 0, line, 0, 0);     // related to step 5
	//printString("\r\n\0");
    makeInterrupt21();                    // Project08 step 1
    //interrupt(0x21, 3, "messag\0", buffer, 0);    // Project08 step 1
    //interrupt(0x21, 0, buffer, 0, 0);             // Project08 step 1
    //printString("\r\n\0");
    interrupt(0x21, 6, "tstprg\0", 0x2000, 0);    // Project08 step 2
    printString("\r\n\0");
    //interrupt(0x21, 7, "tstpr2\0", 0x2000, 0);    // Project08 step 3
    //printString("\r\n\0");
    //interrupt(0x21, 6, "shell\0", 0x2000, 0);       // Project08 step 4
    //interrupt(0x21, 7, 0, 0, 0);                    // Project08 step 4
 	while(1) {
 		//todo
 	}
	return 0;
} //<----------------------------------- Main ends------------------------------------------->


//-------------------------------------printString Function---------------------------------->
void printString(char* String) {
	int i = 0;
	char c = String[i];
	while (c != 0) {
		interrupt(0x10, 0xE * 256 + c, 0, 0, 0);          //interrupt for printing on the screen.
		c = String[++i];
	}
}//-------------------------------------printString Ends------------------------------------->


//-------------------------------------readString Function---------------------------------->
void readString(char* String) {
	int index = 0;
	char c = 0;

	while(1) {
		c = interrupt(0x16, 0, 0, 0, 0);          // Interrupt for Accepting from keyboard,
		interrupt(0x10, 0xE * 256 + c, 0, 0, 0);  // Interrupt for printing on screen.
		if(c == 0xD) {                            // if ENTER
			String[index] = '\n';
		    String[index + 1] = 0;
			break;
		}
			
		if(c == 0x8) {                            // if BACKSPACE
			if(index > 0) {
				index = index -1; 	
			}		
    	}

		String[index] = c;
		++index;		
	}
}//-------------------------------------readString ends------------------------------------->


//-------------------------------------readSector Function---------------------------------->
void readSector(char* buffer, int sector) {
	int CL = mod(sector, 18);   
	int DH = mod(sector/18, 2);
	int CH = sector/36;

	interrupt(0x13, 2 * 256 + 1, buffer, CH * 256 + CL + 1, DH * 256 + 0);         // interrupt for reading a sector.

}//-------------------------------------readSector Ends------------------------------------->


//-------------------------------------Modulo Function-------------------------------------->
int mod(int a, int b) {
	while(a >= b)
		a = a - b;
	return a;
}//-------------------------------------Modulo ends----------------------------------------->



//-------------------------------------handleInterrupt21 Function---------------------------->
void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if(ax == 0){
		printString(bx);
	}else if(ax == 1) {
		printString("Enter a line: ");
		readString(bx);
	} else if(ax == 2) {
		readSector(bx, cx);
	} else if(ax == 3) {
		readFile(bx, cx);
	} else if(ax == 6) {
		executeProgram(bx, cx);
	} else if(ax == 7) {
		terminate();
	} else {
		printString("ERROR! Enter again...\0\n");
	}
}//-------------------------------------handleInterrupt21 ends------------------------------->

//-------------------------------------readFile starts--------------------------------------->
void readFile(char* filename, char* buffer) {                   // Used for reading file from memory.
	char dir[512];
	int index = 0;
	int max = 16;                                                  // max 16 bytes file can be stored. 
	readSector(dir, 2);
	output = 0;
	output = ifFound(dir, index*32, filename);                    //every directory entry is 32 bytes.
	while(output != 1 && index < max) {
		index++;
		output = ifFound(dir, index*32, filename);
	}

	if(output != 1) {
		printString("Woops!! File doesn't exist\n\0");
	} else {
		addFile(dir, index*32, buffer);
	}
}
//-------------------------------------readFile ends----------------------------------------->

//-------------------------------------ifFound starts---------------------------------------->
int ifFound(char* dir, int index, char* name) {        //Finds the message.txt in the folder.
	output = 1;
	j = 0;
	for(j = 0; j < 6; j++) {
		if(name[j] == '\0' && dir[j+index] == '\0' && j != 0) {
	      return output;	
		}
	    
	    if(dir[j+index] != name[j]) {
	    	output = 0;
	    }	
	}

	return output;
	
}
//-------------------------------------ifFound ends--------------------------------------->

//-------------------------------------addFile Starts------------------------------------->
void addFile(char* dir, int index, char* buffer) {            // Adds the file in the sector.
	int i = 6;                                               // 6 bytes for each file. 
	int j = 0;
	char sectorBuffer[512];
	int buffer_index = 0;
	int sector_index = 0;
	while(dir[index+i] != 0x00 && i < 26) {             //Disk divided into 26 sectors.
		sector_index = dir[index+i];
		readSector(sectorBuffer, sector_index);
		for(j = 0; j < 512; j++) {
			buffer[buffer_index]= sectorBuffer[j];
			buffer_index++;
		}
		i++;
	}

}
//-------------------------------------addFile ends-------------------------------------->

//-------------------------------------executeProgram starts----------------------------->
void executeProgram(char* name, int segment) {              // Loads and executes the file from memory.
	char buffer[13312];
	int i = 0;

	readFile(name, buffer);
	printString("Executing the shell: \n");
	while(i < 13312) {
		putInMemory(segment, i, buffer[i]);
		i++;
	}
	launchProgram(segment);                                // Uses the predefined launchProgram from kernel.asm
}
//-------------------------------------executeProgram ends------------------------------->
/*
//-------------------------------------terminate starts---------------------------------->
void terminate() {                                         
	interrupt(0x21, 6, "shell\0", 0x2000, 0);
	}
//-------------------------------------terminate ends------------------------------------>
*/