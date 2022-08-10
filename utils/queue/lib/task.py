import logging
import platform

from time import sleep

import zmq

from lib.exceptions import TaskException

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s [%(levelname)s] %(message)s')


class TaskServer:
    """ Server """
    def __init__(self):
        self.logger = logging.getLogger(self.__class__.__name__)
        self._tasks = []
        self._to_compare = []
        self.socket = None
        self.running = False

    def __str__(self):
        ret = []
        i = 0
        for t in self._tasks:
            i += 1
            ret.append(f'Task #{i:03}')
            for k, v in t.items():
                ret.append('    {} → {}'.format(k, v))
        return '\n'.join(ret)

    def start(self, host='127.0.0.1', port='5555'):
        self.running = True
        self.logger.info(f'Task Server running on {host}:{port}')

        context = zmq.Context()
        self.socket = context.socket(zmq.REP)
        self.socket.bind(f'tcp://{host}:{port}')

    def add_to_compare(self, path):
        if path not in self._to_compare:
            self._to_compare.append(path)

    def get_compare(self):
        for p in self._to_compare:
            return p
        return False

    @property
    def to_compare(self):
        return self._to_compare

    @to_compare.setter
    def to_compare(self, cmp):
        self._to_compare = cmp

    def process_task(self):
        if self.running:
            try:
                message = self.socket.recv_json()
                self.logger.debug(f'Received message from {message["worker_info"]["node"]} ({message["worker_info"]["id"]})')
            except ValueError:
                strerr = 'Wrong messsage format, could not decode JSON'
                self.logger.error(strerr)
                self.socket.send_json({'type': 'error', 'error': strerr})
                raise TaskException(strerr)

            try:
                mtype = message['type']
                worker_info = message['worker_info']
            except KeyError as e:
                strerr = "Wrong message! Missing key: {}".format(e.message)
                self.logger.error(strerr)
                self.socket.send_json({'type': 'error', 'error': strerr})
                raise TaskException(strerr)

            if mtype == 'task':
                if (t := self.get_task(worker_info['system'])):
                    msg = {'type': 'task', 'task': t}
                    self.socket.send_json(msg)
                    self._tasks.remove(t)
                else:
                    self.socket.send_json({'type': 'queue_empty'})

            elif mtype == 'compare':
                self.logger.info('Compare request')
                if (path := self.get_compare()):
                    msg = {'type': 'compare', 'compare': path}
                    self._to_compare.remove(path)
                    self.socket.send_json(msg)
                else:
                    self.socket.send_json({'type': 'queue_empty'})

            elif mtype in ('result', 'compare_result'):
                self.logger.info('Result received')
                self.socket.send_json({'type': 'ack'})
                return message

    def stop(self):
        self.running = False
        self.logger.info('Server terminated.')

    @property
    def tasks(self):
        return self._tasks

    def get_task(self, system):
        for t in self._tasks:
            if t['system'].lower() == system.lower():
                return t
        return False

    @tasks.setter
    def tasks(self, tasks):
        self._tasks = tasks


class TaskWorker:
    """ Worker """
    def __init__(self, worker_id):
        self.logger = logging.getLogger(self.__class__.__name__)

        self.info = {
                'id': worker_id,
                'node': platform.node(),
                'system': platform.system()
                }

        self.context = None
        self.zmq_socket = None

        self.running = False

    def __str__(self):
        s = ''
        for k, v in self.info.items():
            s += f'{k}={v}; '
        return s

    def start(self, callback=print, task_type='task', srvhost='127.0.0.1', srvport='5555'):
        self.running = True

        try:
            self.logger.info(f'Connecting to server on {srvhost}:{srvport}')

            self.context = zmq.Context()
            self.zmq_socket = self.context.socket(zmq.REQ)
            #self.zmq_socket.setsockopt(zmq.RCVTIMEO, 1000)
            #Do not wait infinitely for socket shutdown
            #self.zmq_socket.setsockopt(zmq.LINGER, 2000)
            self.zmq_socket.connect(f'tcp://{srvhost}:{srvport}')

            while self.running:
                req_msg = {'type': task_type, 'worker_info': self.info}
                self.zmq_socket.send_json(req_msg)
                message = self.zmq_socket.recv_json()

                mtype = message['type']
                if mtype == 'task' or mtype == 'compare':
                    if mtype == 'task':
                        task = message['task']
                    elif mtype == 'compare':
                        task = message['compare']

                    result = callback(task)
                    result_msg = {'worker_info': self.info}

                    if mtype == 'task':
                        result_msg['type'] = 'result'
                        result_msg['target'] = task['target']
                    elif mtype == 'compare':
                        result_msg['type'] = 'compare_result'
                        result_msg['path'] = task

                    result_msg['data'] = result

                    self.zmq_socket.send_json(result_msg)
                    r = self.zmq_socket.recv_json()
                    if r['type'] != 'ack':
                        self.logger.error(r['error'])
                    #else:
                    #    self.logger.debug('ACK')

                elif mtype == 'queue_empty':
                    self.logger.debug('Queue empty')
                    sleep(1)

                elif mtype == 'error':
                    self.logger.error(message)

        #except zmq.error.Again as e:
        #    raise ConnectionError(e.strerror + ' - server down?')
        except KeyboardInterrupt:
            self.logger.info('SIGINT received')
            self.stop()

    def stop(self):
        self.zmq_socket.close()
        self.context.term()
        self.running = False
        self.logger.info('Worker finished.')
