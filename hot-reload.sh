#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Usage: $0 <file_path> [<directory_path>]"
	exit 1
fi

compile_backend() {
	echo "Compiling..."
	cmake .
	make
}

run_backend() {
	"$1" &
	BACKEND_PID=$!
	echo "Running process with PID: $BACKEND_PID"
}

monitor_changes() {
	echo "Monitoring for changes..."
	while true; do
		inotifywait -r -e modify,move,create,delete ./
		kill_backend
		compile_backend
		run_backend $1
	done
}

kill_backend() {
	if [[ -n $BACKEND_PID ]]; then
		echo "Killing process with PID: $BACKEND_PID"
		kill $BACKEND_PID
		wait $BACKEND_PID 2>/dev/null
	fi
}

main() {
	if [ -d "$2" ]; then
		cd "$2"	|| exit 1
	fi

	compile_backend
	run_backend $1
	monitor_changes $1
}

main $1 $2
