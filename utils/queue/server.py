#!/usr/bin/env python
import os
import base64
import uuid
import random
import string

from datetime import datetime

from lib.task import TaskServer
from lib.exceptions import TaskException

RESULTS_DIR = 'results'
RUN_TS = str(int(datetime.timestamp(datetime.now())))
RUN_TS = '1660060837'

targets = set()
targets_done = set()

def random_task():
    #rnd_target = random.choice(['openssl', 'libressl'])
    #rnd_sys = random.choice(['linux', 'freebsd', 'openbsd'])
    #seed = str(random.randint(0, 999999999)).zfill(9)
    seed = '1234'
    task = {'target': 'toy', 'system': 'linux', 'seed': seed}

    return task

def toy_task(seed):
    task = {'target': 'minerva_lib/target/toy/bin/minerva-toy-toy', 'system': 'linux', 'seed': str(seed)}
    return task

def poll(current_tasks=[], queue_size=10):
    if len(current_tasks) <= queue_size:
        tasks = []
        #for i in range(0, random.randint(0, queue_size)):
        #    tasks.append(random_task())
        for i in range(3):
            seed = random.randint(1000, 2999)

            # TUTAJ DODAĆ ZADANIE
            t = toy_task(seed)

            tasks.append(t)
            targets.add(t['target'])
        return current_tasks + tasks

    return current_tasks


def topath(name):
    clear_name = ''
    for n in name:
        if n in string.ascii_letters + string.digits:
            clear_name += n
        else:
            clear_name += '_'
    return clear_name


if __name__ == '__main__':
    ts = TaskServer()

    ts.start('127.0.0.1')

    ts.tasks = poll(ts.tasks)
    while True:
        #ts.tasks = poll(ts.tasks)
        #print(ts)

        if len(ts.tasks) == 0:
            for target in targets:
                if target not in targets_done:
                    ts.add_to_compare(os.path.join(RESULTS_DIR, RUN_TS,
                        topath(target)))
                    targets_done.add(target)

        try:
            r = ts.process_task()
            if r:
                if r['type'] == 'result':
                    result = r['data']
                    fname = '{system}@{worker}_{uniq}'.format(
                            system=r['worker_info']['system'].lower(),
                            worker=r['worker_info']['id'],
                            uniq=uuid.uuid4().hex
                            )

                    results_path = os.path.join(RESULTS_DIR, RUN_TS,
                                                topath(r['target']),
                                                topath(result['seed']))
                    os.makedirs(results_path, exist_ok=True)

                    if result['ret'] == 0:
                        with open(os.path.join(results_path, fname) + '.out', 'wb') as fd:
                            fd.write(base64.b64decode(result['stdout']))
                    else:
                        with open(os.path.join(results_path, fname) + '.err', 'wb') as fd:
                            fd.write(base64.b64decode(result['stderr']))
                if r['type'] == 'compare_result':
                    if r['data']['cmpresult'] != 'error':
                        print(r['data']['cmpresult'])
                    else:
                        print('Error:', r['data']['error'])

        except TaskException as e:
            print('ERROR: {}'.format(e))
        except KeyboardInterrupt:
            ts.stop()
            break
