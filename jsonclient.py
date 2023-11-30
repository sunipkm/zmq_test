# %% Imports
import json
import time
import zmq
import signal
import zmq.utils.monitor as zmonitor
from datetime import datetime
# %% Signal Handler
    
def block_until_available(sock, ehandler):
    while True:
        try:
            evt = sock.recv_multipart()
            evt = zmonitor.parse_monitor_message(evt)
            if evt['event'] == zmq.EVENT_CONNECTED:
                print('Connected')
                break
        except zmq.error.Again:
            time.sleep(1)
            continue

# %%
ctx = zmq.Context()
sock = ctx.socket(zmq.REQ)
sock.setsockopt(zmq.RCVTIMEO, 1000)
sock.setsockopt(zmq.REQ_CORRELATE, 1) # allow reconnection after drop
sock.setsockopt(zmq.REQ_RELAXED, 1) # allow reconnection after drop
sock.setsockopt(zmq.LINGER, 0) # allow exit
# mon = sock.get_monitor_socket(zmq.EVENT_ALL)
sock.connect("tcp://localhost:5555")
packet = {
    'cmd_type': 'get',
    'cam_id': 'DEV_DEADBEEF',
    'command': 'image_size',
    'params': [],
}

poller = zmq.Poller()
poller.register(sock, zmq.POLLIN)

while True:
    try:
        packet['params'] = [datetime.now().strftime('%H:%M:%S')]
        sock.send(json.dumps(packet).encode('utf-8'))
    except Exception as e:
        print(f'Exception: {e}')
        time.sleep(1)
        continue
    try:
        obj = dict(poller.poll(1000))
        if sock in obj and obj[sock] == zmq.POLLIN:
            reply = sock.recv()
            print(f'Raw reply: {reply}')
            packet = json.loads(reply)
            print(f'JSON reply: {packet}')
            time.sleep(1)
        else:
            print('Timeout')
    except KeyboardInterrupt:
        break
sock.close()
ctx.term()
# %%
