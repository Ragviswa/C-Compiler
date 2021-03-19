#!/bin/bash

if which dos2unix ; then
	DOSUNIX="dos2unix"
elif which fromdos ; then
	DOSUNIX="fromdos"
else
	DOSUNIX="cat"
fi

echo "============================="
PASSED=0
CHECKED=0

make clean
make bin/c_compiler
mkdir -p testing
OUTPUT_DIRECTORY=testing

for i in compiler_tests/*; do
	SUB_FOLDER=${OUTPUT_DIRECTORY}/$(basename ${i})
	mkdir -p ${SUB_FOLDER}
	for j in $i/*.c; do
		b=$(basename ${j});
		n=${b%.c}
		m=${j%.c}

		if [[ $b != *_driver.c ]]; then
			TESTCASE=${SUB_FOLDER}/${n}
			mkdir -p ${TESTCASE}
			echo "============================="
			echo ""
			echo "Input file : ${j}"
			echo "Testing $n"

			# run the compiler on the test program
			bin/c_compiler -S ${j} -o ${TESTCASE}/$n.s

			# use GCC to assemble the generated assembly program
			mips-linux-gnu-gcc -mfp32 -o ${TESTCASE}/$n.o -c ${TESTCASE}/$n.s

			# use GCC to link the generated object file with the driver program to produce an executable
			mips-linux-gnu-gcc -mfp32 -static -o ${TESTCASE}/$n ${TESTCASE}/$n.o ${m}_driver.c

			# simulate the executable on MIPS
			qemu-mips ${TESTCASE}/$n
		fi	
	done
done