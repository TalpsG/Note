NODE1_PASSWORD=1234

function LOG_INFO() {
	printf "$(date +%Y-%m-%d\ %T)  $0  [ INFO  ]  %s\n" "$@"
}

function LOG_WARN() {
	printf "$(date +%Y-%m-%d\ %T)  $0  [ WARN  ]  %s\n" "$@"
}

function LOG_ERROR() {
	printf "$(date +%Y-%m-%d\ %T)  $0  [ ERROR ]  %s\n" "$@"
}
function CHECK_RESULT() {
	actual_result=$1
	expect_result=${2-0}
	mode=${3-0}
	error_log=$4
	exit_mode=${5-0}

	if [ -z "$actual_result" ]; then
		LOG_ERROR "Missing actual error code."
		return 1
	fi

	if [ $mode -eq 0 ]; then
		test "$actual_result"x != "$expect_result"x && {
			test -n "$error_log" && LOG_ERROR "$error_log"
			((exec_result++))
			LOG_ERROR "${BASH_SOURCE[1]} line ${BASH_LINENO[0]}"
			if [ $exit_mode -eq 1 ]; then
				exit 1
			fi
		}
	else
		test "$actual_result"x == "$expect_result"x && {
			test -n "$error_log" && LOG_ERROR "$error_log"
			((exec_result++))
			LOG_ERROR "${BASH_SOURCE[1]} line ${BASH_LINENO[0]}"
			if [ $exit_mode -eq 1 ]; then
				exit 1
			fi
		}
	fi

	return 0
}
function pre_test() {
	LOG_INFO "Start environmental preparation."
	grep "^example:" /etc/passwd && userdel -rf example
	local_umask=$(umask)
	umask | grep 0022 || umask 0022
	LOG_INFO "End of environmental preparation!"
}

function run_test() {
	LOG_INFO "Start executing testcase."
	useradd example
	passwd example <<EOF
${NODE1_PASSWORD}
${NODE1_PASSWORD}
EOF
	touch /tmp/my_pwd
	ls -l /tmp/my_pwd | grep '\-rw\-r\-\-r\-\-.'
	CHECK_RESULT $? 0 0 "Check file default umask failed"
	setfacl -m u:example:w /tmp/my_pwd
	CHECK_RESULT $? 0 0 "Failed to add a user permission for 'example'"
	getfacl /tmp/my_pwd >tmp_log
	echo '# file: tmp/my_pwd
# owner: root
# group: root
user::rw-
user:example:-w-
group::r--
mask::rw-
other::r--
' >diff_file
	diff tmp_log diff_file
	CHECK_RESULT $? 0 0 "Comparison between tmp_log and diff_file failed"
	ls -l /tmp/my_pwd | grep '\-rw\-rw\-r\-\-+'
	CHECK_RESULT $? 0 0 "Check file umask failed"
	su - example -c 'echo test>/tmp/my_pwd'
	CHECK_RESULT $? 0 0 "Switching to 'example' failed to execute echo command"
	LOG_INFO "Finish testcase execution."
}

function post_test() {
	LOG_INFO "start environment cleanup."
	rm -rf tmp_log diff_file /tmp/my_pwd
	userdel -rf example
	umask $local_umask
	LOG_INFO "Finish environment cleanup!"
}

pre_test
run_test
post_test
