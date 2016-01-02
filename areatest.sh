#! /bin/sh
function check_area() {
  local BOOL=0
  for X in area/*; do
    local BOOL=0
    for Y in $(cat area/area.lst); do 
      if [ $X = "area/$Y" ]; then 
        BOOL=1;
        break;
      fi
    done
    if [ $BOOL != 1 ]; then
      let NUM=$NUM+1
      eval echo $X | cut -c6-; fi 
  done
}

function header() {
echo "Files that can be deleted because they are not in area.lst:"
echo "-----------------------------------------------------------"
}

function footer() {
echo "-----------------------------------------------------------"
echo "$NUM files"
}

NUM=0 
cd /home/ur/ur
header
check_area
footer
