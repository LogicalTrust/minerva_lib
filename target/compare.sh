#! /bin/sh

LEFT=$1
RIGHT=$2
ITER=$3

OSTYPE=$(uname -s)

if [ "$OSTYPE" = "Linux" ]; then
	MD5=md5sum
else
	MD5=md5
fi

while true; do 
    while true; do
        SEED=$(od -A n -t u -N 4 /dev/urandom | tr -d ' ')
        LEFT_RESULT=$(${LEFT} -i ${ITER} -s ${SEED} -D /dev/stdout | sort -n | ${MD5})
        RIGHT_RESULT=$(${RIGHT} -i ${ITER} -s ${SEED} -D /dev/stdout | sort -n | ${MD5})

        if [ "${LEFT_RESULT}" != "${RIGHT_RESULT}" ] ; then
            break
        fi
    done

    echo "Found difference: ${SEED}"
    echo "LEFT: ${LEFT} -i ${ITER} -s ${SEED} -D /dev/stdout"
    echo "RIGHT: ${RIGHT} -i ${ITER} -s ${SEED} -D /dev/stdout"
    echo ""
    ${LEFT} -i ${ITER} -s ${SEED} -D $4${LEFT_RESULT}_${RIGHT_RESULT}.left -T $4${LEFT_RESULT}_${RIGHT_RESULT}.trace
    ${RIGHT} -i ${ITER} -s ${SEED} -D $4${LEFT_RESULT}_${RIGHT_RESULT}.right
    echo ${SEED} > $4${LEFT_RESULT}_${RIGHT_RESULT}.seed
done
