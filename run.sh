# Compiler driver

# echo $(basename $1 .c).i

mkdir -p ./build/tests/
# -E only preprocessor -P no line numbers
gcc -E -P $1 -o ./build/tests/$(basename $1 .c).i
# gen asm
./build/mycc ./build/tests/$(basename $1 .c).i ./build/tests/$(basename $1 .c).s

# if -S flag, exit after asm generation
if [ $2 -eq "-S" ]; then
    # assemble
    gcc ./build/tests/$(basename $1 .c).s -o ./build/tests/$(basename $1 .c)
    # cleanup
    rm ./build/tests/$(basename $1 .c).i
    # run and print return value
    ./build/tests/$(basename $1 .c)
    echo $?
fi

