mkdir -p build/tests
cd build
cmake -B . -S ../ && cmake --build .
cd ..

for src_file in `ls ../tests`
do
    echo TESTING $src_file
    # cat ../tests/$src_file | ./mycc | tee ./tests/$src_file.tokens
    ./run.sh ./tests/$src_file
    # compare program outputs when more steps added
done

#check for leaks with 
# gcc -g -O0 -fsanitize=address,undefined -Wall -Wextra ../src/main.c ../src/lexer/token.c ../src/lexer/lexer.c ../src/parser/parser.c ../src/parser/ast.c ../src/codegen/codegen.c ../src/codegen/asm.c ../src/code_emission/emission.c -o ./mycc