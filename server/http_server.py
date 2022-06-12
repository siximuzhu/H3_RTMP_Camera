from flask import Flask
from flask import jsonify
from flask import request
import json
import sys
import cv2 as cv
import time
import base64


test_str = 'hello world!'
# 编码
encode_str = base64.encodebytes(test_str.encode('utf8'))  # b'aGVsbG8gd29ybGQh\n'
print(encode_str.decode())  # 默认以utf8解码，结果 aGVsbG8gd29ybGQh
# 解码
decode_str = base64.decodebytes(encode_str)  # b'hello world!'
print(decode_str.decode())  # 默认以utf8解码，结果 hello world!



app = Flask(__name__)

print("start")

@app.route('/')
def index():
    return '<h1>Hello,World123121233!</h1>'


@app.route("/data",methods=['POST'])
def stream():
    data = request.get_data()
    
    data = json.loads(data)
    board_serial = data['board_serial']
    counter = data['counter']
    power = data['power']

    f = open('log.txt','a')
    f.write(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())) + '\n')
    f.write('板子序列号：' + board_serial + '\n')
    f.write('信号次数：' + counter + '\n')
    f.write('当前电量：' + power + '%\n\n')
    f.close()

        
    return jsonify({"status":"ok"})
    


if __name__ == '__main__':
    app.run(host='0.0.0.0',port=80,debug=True)
