##########################################################
# Description: Template to build the makefile for
#   "trab01".
#
# Author: Edson Borin - edson@ic.unicamp.br
#
# 1) You may add more files to your project changing the 
#    OBJ_FILES list.
#
# 2) You may remove or add new rules to compile your .c files 
#    (see as an example rules main.o and error.o)
#
# 3) You may not change the ias-as rule (do not add 
#    extra libraries!)
#
##########################################################

# List of object files that must be linked together to 
# build the ias-as executable
OBJ_FILES=main.c hashtable.c iasparser.c

# Default rule
all: ias-as

# -- Clean rule. It removes the ias-as file and any .o file on the directory --
clean:
	rm -f ias-as *.o

##########################################################
# Do not change this rule
##########################################################

# -- Rule to make the executable ias-as
ias-as: $(OBJ_FILES)
	gcc -g -std=c99 $(OBJ_FILES) -o ias-as 
