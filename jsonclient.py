# %% Imports
import json
import zmq
import signal
# %% Signal Handler
class GracefulInterruptHandler(object):

    def __init__(self, sig=signal.SIGINT):
        self.sig = sig

    def __enter__(self):

        self.interrupted = False
        self.released = False

        self.original_handler = signal.getsignal(self.sig)

        def handler(signum, frame):
            self.release()
            self.interrupted = True

        signal.signal(self.sig, handler)

        return self

    def __exit__(self, type, value, tb):
        self.release()

    def release(self):

        if self.released:
            return False

        signal.signal(self.sig, self.original_handler)

        self.released = True

        return True

# %%
ctx = zmq.Context()
sock = ctx.socket(zmq.REQ)
sock.setsockopt(zmq.RCVTIMEO, 1000)
sock.connect("tcp://localhost:5555")
packet = {
    'cmd_type': 'get',
    'cam_id': 'DEV_DEADBEEF',
    'command': 'image_size',
    'params': [],
}
with GracefulInterruptHandler() as h:
    while not h.interrupted:
        try:
            sock.send(json.dumps(packet).encode('utf-8'))
        except Exception as e:
            print(f'Exception: {e}')
        try:
            reply = sock.recv()
            print(f'Raw reply: {reply}')
            packet = json.loads(reply)
            print(f'JSON reply: {packet}')
        except zmq.error.Again:
            print('Timeout')
sock.close()
ctx.term()
# %%
