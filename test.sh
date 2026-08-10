mkdir -p build/tests
cd build
cmake -B . -S ../ && cmake --build .

for src_file in `ls ../tests`
do
    echo TESTING $src_file
    cat ../tests/$src_file | ./mycc | tee ./tests/$src_file.tokens
    # compare program outputs when more steps added
done