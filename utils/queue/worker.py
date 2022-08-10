#!/usr/bin/env python
import sys
import subprocess
import base64

from lib.task import TaskWorker


def process_task(task):
    cmd = [task['target'], '-T', '/dev/stdout', '-s', task['seed']]
    p = subprocess.run(cmd, capture_output=True)

    return {'args': '_'.join(p.args),
            'seed': task['seed'],
            'ret': p.returncode,
            'stdout': base64.b64encode(p.stdout).decode(),
            'stderr': base64.b64encode(p.stderr).decode()}


if __name__ == '__main__':
    try:
        worker_name = sys.argv[1]
    except IndexError:
        worker_name = 'test-worker-1'

    worker = TaskWorker(worker_name)
    try:
        worker.start(callback=process_task, srvhost='127.0.0.1')
    except ConnectionError as e:
        sys.exit(e)
    except KeyboardInterrupt:
        worker.stop()
        sys.exit(0)
