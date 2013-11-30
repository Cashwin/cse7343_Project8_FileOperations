/* kernel.c
   A simple shell that displays a message and executes a Program.
   Ashwin Chandrasekhar and Preethi Rajagopalan
*/

int find(char * command);                                          // Finds the file in loaded in memory.
int compare(char * String1, char * String2);                       // Used for comparing two Strings.
void storeArgument(char * originalString, char * String, int num); // Used to splitting the command entered.

//<---------------------------------- Main Function-------------------------------->
int main() {
    char command[90];
    int x;
    int commandNum = 0;
    char arg0[45];
    char arg1[45];
    char buffer[13312];

     while(1) {
        for(x = 0; x < 90; x++) {
            command[x] = 0x0;
            if(x < 45){
                arg0[x] = 0x0;
                arg1[x] = 0x0;
           }

        }

        interrupt(0x21, 0, "SHELL STARTS\0\n", 0, 0);                    // For Printing.
        interrupt(0x21, 0, ">", 0, 0);                                   // For Printing.

        interrupt(0x21, 1, command, 0, 0);                               // For readString.
        storeArgument(command, arg0, 0);
        commandNum = find(arg0);

        if(commandNum == 0) {
            interrupt(0x21, 0, "Command not found!\n\0", 0, 0);
            break;
        } else if(commandNum == 1) {
            storeArgument(command, arg1, 1);
            interrupt(0x21, 3, arg1, buffer, 0);
            interrupt(0x21, 0, buffer, 0, 0);
            break;
        } else if(commandNum == 2) {
            storeArgument(command, arg1, 1);
            interrupt(0x21, 6, arg1, 0x2000, 0);
            break;
        } else {
            break;
        }    
      }

    return 0;
}

//<------------------------------------Main Ends------------------------------------>


//<------------------------------------Find Function-------------------------------->
int find(char* command) {
    char* command1 = "type\0";          
    char* command2 = "execute\0";
    if(compare(command, command1) == 1) {                   // if type.
        return 1;
    } else if(compare(command, command2) == 1) {            // if exeecute.
        return 2;
    } else {
        return 0;
    }
    return 0;
}
//<------------------------------------Find Ends------------------------------------>


//<------------------------------------storeArgument Function----------------------->
void storeArgument(char * originalString, char * String, int num) {
    int i = 0;
    int j = 0;
    int Arg = 0;

    while(Arg != num) {
        while(originalString[i] != ' ') {
            if(originalString[i] == 0x0) {
                interrupt(0x21, 0, "Invalid command!\n\0", 0, 0);
                return;
            }
            i++;
        }
        i++;
        Arg++;
    }
    
    while(originalString[i] != ' ' && originalString[i] != 0x0) {        // copy originalString to String.
        String[j] = originalString[i];
        i++;
        j++;
    }
    String[j] = 0x0;
}
//<------------------------------------storeArgument Ends---------------------------->


//<------------------------------------Compare Function------------------------------>
int compare(char* String1, char* String2) {
    int i = 0;
    int output = 1;

    while(String2[i] != '\0' || String1[i] != '\0') {         // run till one of the array is \0
        if(String1[i] != String2[i]){
            return 0;
        }
        i++;

    }

    return output;
}
//<------------------------------------Compare Ends---------------------------------->
