#!/bin/sh
tstlog=test.log
test -f $tstlog && rm $tstlog

tstdir=test.dir
test -d $tstdir || mkdir $tstdir

run_unit_test () {
    test=$1
    stdouto=${tstdir}/${test}.stdout
    stderro=${tstdir}/${test}.stderr
    stdoutf=${stdouto}.current
    stderrf=${stderro}.current

    rm -f ${stdoutf} ${stderrf}

    echo "Running test: "${test} | tee -a ${tstlog}

    ./${test} 1> ${stdoutf} 2> ${stderrf}
    res=$?

    test $res -eq 0 || echo " - result: "$res | tee -a ${tstlog}

    if test -f ${stdouto} -o -f ${stderro}; then
	if test -f ${stdouto}; then
	    diff -q ${stdouto} ${stdoutf}
	    if test $? -ne 0 ; then
		echo " - stdout differ" | tee -a ${tstlog}
	    else
		rm ${stdoutf}
	    fi
	    
	    diff -q ${stderro} ${stderrf}
	    if test -$? -ne 0 ; then
		echo " - stderr differ" | tee -a ${tstlog}
	    else
		rm ${stderrf}
	    fi
	fi
    else
	mv ${stdoutf} ${stdouto}
	mv ${stderrf} ${stderro}
    fi
}

show_diffs() {
    test=$1
    stdouto=${tstdir}/${test}.stdout
    stderro=${tstdir}/${test}.stderr
    stdoutf=${stdouto}.current
    stderrf=${stderro}.current

    if test -f $stdoutf; then
	diff $stdouto $stdoutf
    fi

    if test -f $stderrf; then
	diff $stderro $stderrf
    fi
}

if test "$1" = "--show-diff"; then
    shift
    while test _$1 != _; do
	show_diffs $1
	shift
    done
else
    while test _$1 != _; do
	run_unit_test $1
	shift
    done
fi
