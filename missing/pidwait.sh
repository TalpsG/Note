pidwait(){
	kill -0 $1
	ret=$?
	while((ret==0))
	do
		sleep 1
		kill -0 $1
		ret=$?
	done
	echo "pid:$1 has finish"
}
