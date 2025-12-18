cd lexer
make
cd ..

rm -rf build/tests
mkdir -p build/tests

for src_file in `ls -d tests/*`
do
    echo $src_file
    cat $src_file | lexer/build/cc > build/$src_file.tokens
done