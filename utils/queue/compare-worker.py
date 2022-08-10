#!/usr/bin/env python
import sys
import subprocess
import base64
import filecmp
import os
import itertools
import filecmp
import shutil
import difflib

from lib.task import TaskWorker
from server import topath

OUTPUT_PATH = "compare_output"

def compare_files(a, b):
    return filecmp.cmp(a, b)


def compare_dir(path):
    if path:
        seeds = [d for d in os.listdir(path) if os.path.isdir(os.path.join(path, d))]
        for seed in seeds:
            fnames = []
            for f in os.listdir(os.path.join(path, seed)):
                fname = os.path.join(path, seed, f)
                if os.path.isfile(fname):
                    fnames.append(fname)

            diff = []
            # get different files
            for a, b in list(itertools.combinations(fnames, 2)):
                if not compare_files(a, b):
                    diff.append(a)
                    diff.append(b)

            # copy different files to outpud directory
            if len(diff) > 0:
                dest = os.path.join(OUTPUT_PATH, str(seed).zfill(10))
                os.makedirs(dest, exist_ok=True)
                for src in diff:
                    shutil.copy(src, os.path.join(dest, topath(src)))

                # save diff
                fd0 = open(diff[0]).readlines()
                fd1 = open(diff[1]).readlines()
                outdiff = difflib.unified_diff(fd0, fd1, diff[0], diff[1])
                with open(os.path.join(dest, 'diff'), 'w') as f:
                    f.write(''.join(outdiff))

        return {'cmpresult': 'ok', 'diff': ''}
    else:
        return {'cmpresult': 'error', 'error': f'Path not provided'}

if __name__ == '__main__':
    try:
        worker_name = sys.argv[1]
    except IndexError:
        worker_name = 'compare-worker-1'

    worker = TaskWorker(worker_name)
    try:
        worker.start(task_type='compare', callback=compare_dir, srvhost='127.0.0.1')
    except ConnectionError as e:
        sys.exit(e)
    except KeyboardInterrupt:
        worker.stop()
        sys.exit(0)
