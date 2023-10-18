target = test

# vpath %.cpp ./interfaces \
# 		./mutators/%_mutators/%_mutateFrom ./mutators/%_mutators/%_mutateTo

vpath %.cpp ./interfaces \
		./mutators/AOR_mutators/AOR_mutateFrom ./mutators/AOR_mutators/AOR_mutateTo \
		./mutators/UOI_mutators/UOI_mutateFrom ./mutators/UOI_mutators/UOI_mutateTo \
		./mutators/LOR_mutators/LOR_mutateFrom ./mutators/LOR_mutators/LOR_mutateTo \
		./mutators/ROR_mutators/ROR_mutateFrom ./mutators/ROR_mutators/ROR_mutateTo \
		./mutators/COR_mutators/COR_mutateFrom ./mutators/COR_mutators/COR_mutateTo \
		./mutators/ABS_mutators/ABS_mutateFrom ./mutators/ABS_mutators/ABS_mutateTo \
		./mutators/CON_mutators/CON_mutateFrom ./mutators/CON_mutators/CON_mutateTo \
		./mutators/UOD_mutators/UOD_mutateFrom ./mutators/UOD_mutators/UOD_mutateTo
vpath %.o ./obj

INTERFACE := UOD
FROM := NEGATE
TO := NEGATE

START_LINE := -fplugin-arg-plugin-start_line=2
END_LINE := -fplugin-arg-plugin-end_line=71
VAR_NAME := -fplugin-arg-plugin-var_name=f
CCP := g++ -std=c++11

PI_LIB_PATH := -I`g++ -print-file-name=plugin`/include
CFLAGS := -Wall
LDFLAGS := -fno-rtti -Wno-literal-suffix
FPIC := -fPIC
SHARED := -shared

O_TAR := pluginInterface.o $(INTERFACE)Interface.o $(FROM)_1.o $(TO)_2.o

test:plugin.so test.c
	gcc -o test test.c -fplugin=./plugin.so $(START_LINE) $(END_LINE) $(VAR_NAME)

plugin.so: $(O_TAR)
	$(CCP) $(FPIC) $(SHARED) $(O_TAR) -o plugin.so 

%.o:%.cpp
	$(CCP) -c $(PI_LIB_PATH) $(CFLAGS) $(LDFLAGS) $(FPIC) $< -o $@

jug:
	gcc test.c -fplugin=./plugin.so $(START_LINE) $(END_LINE) $(VAR_NAME) -fdump-tree-all-lineno

clean:
	rm *.o ./obj/*.o ./*.so ./*-test* ./*.c.* test a.out
 