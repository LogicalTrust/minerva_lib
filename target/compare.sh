#! /bin/sh

LEFT=$1
RIGHT=$2
ITER=$3

while true; do
    SEED=$(od -A n -t u -N 4 /dev/urandom | tr -d ' ')
    LEFT_RESULT=$(${LEFT} -i ${ITER} -s ${SEED} -D /dev/stdout | sort -n | md5)
    RIGHT_RESULT=$(${RIGHT} -i ${ITER} -s ${SEED} -D /dev/stdout | sort -n | md5)

    if [ "${LEFT_RESULT}" != "${RIGHT_RESULT}" ] ; then
        break
    fi
done

echo "Found difference: ${SEED}"
echo "LEFT: ${LEFT} -i ${ITER} -s ${SEED} -D /dev/stdout"
echo "RIGHT: ${RIGHT} -i ${ITER} -s ${SEED} -D /dev/stdout"
