#!/bin/bash
#-n=no libs
#-l=only libs
#-e=editor
if test -d notes;then
  cd ./notes
  ./notes.sh
  if test $? -ne 0;then
    exit 1
  fi
  cd ..
fi
#^ for developer communication
cmd=""
NCURSES_PATH="/opt/c9/ncurses-6.3/"
echo "">log.txt
args=$1;args=${args:1}
while read -n1 i;do
  if test "$i" = "m" ;then
    echo Compiling main program
    g++ ./main.cc -o main.out -I${NCURSES_PATH}include -L${NCURSES_PATH}lib/ -lm -I./lib -L./lib -l:r@@ui.a -lncurses 2>log.txt
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo Main program compilation error!
      cat log.txt
      exit 1
    fi
    echo Running main program
    cmd+="./main.out ${@:2};"
  elif test "$i" = "e" ;then
    echo Compiling asset editor
    g++ ./lib/tools/editor.cc -o ./lib/tools/editor.out -I${NCURSES_PATH}include -L${NCURSES_PATH}lib/ -lm -I./lib -L./lib -lncurses 2>log.txt
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo Asset editor compilation error!
      cat log.txt
      exit 1
    fi
    echo Running asset editor
    cmd+="./lib/tools/editor.out ${@:2};"
  elif test "$i" = "l";then
    rm ./lib/*.a 2>/dev/null
    echo Compiling libraries
    while read file; do
      #file=line in order.txt
      libs=($file)
      if test ${libs[0]} != "#";then
      greg=(${libs[@]:2})
      #libs=list of tokens in line
      nc=""
      echo "  - ${libs[0]}"
        for i in ${!libs[@]}; do
          if test $i -gt 1; then
            if test ${libs[$i]} = "ncurses";then
              nc="-I${NCURSES_PATH}include -I${NCURSES_PATH}c++ -L${NCURSES_PATH}lib "
            fi
            libs[$i]="-l${libs[$i]}"
          fi
        done
        g++ -c "./lib/src/${libs[0]}.cc" -o "./lib/src/${libs[0]}.o"  ${nc}-I./lib -L./lib ${libs[@]:2} 2>>log.txt
        ar rcs ./lib/${libs[1]}.a ./lib/src/${libs[0]}.o ./lib/${libs[1]}.h 2>>log.txt
      fi
    done <./lib/src/order.txt
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo Library error!
      cat log.txt
      exit 1
    fi
  elif test "$i" = "r";then
    exit
  fi
done < <(echo "$args")
bash -c "$cmd"
exit