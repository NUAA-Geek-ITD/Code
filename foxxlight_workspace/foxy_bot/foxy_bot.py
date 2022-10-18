import asyncio
import json
import os
import re
import threading
import time

import websockets
import requests
from io import BytesIO
from PIL import Image

import webui
from modules.txt2img import txt2img
from modules.img2img import img2img
import paramiko

address = "172.27.1.100"
cq_port = 26711
ssh_port = 22
ssh_username = "root"
ssh_password = "***"
pendingRecv = {}
drawQueue = []
working = False
image_record = {}
loop = None


class DrawTask:
    def __init__(self, response):
        self.response = response

    prompt = "((cg 4k wallpaper)),masterpiece,(highres),,loli,((white cat ears)),wine red eyes,((cat hairclip)), long hair,white hair,look at viewer,small chest,seiza,white leg wear, (((((1girl)))),White off-the-shoulder cardigan,one eye closed,simple background,(presenting armpit),(((straight))),((solo)),full body,white panties,"
    width = 512
    height = 1024
    steps = 30
    seed = -1
    image = None
    response = None


async def network():
    global pendingRecv
    try:
        while True:
            print(f"Connecting to {address}")
            async with websockets.connect(f"ws://{address}:{cq_port}") as ws:
                # user_task = asyncio.create_task(user(ws))
                trans = paramiko.Transport(address, ssh_port)
                trans.connect(username=ssh_username, password=ssh_password)
                sftp = paramiko.SFTPClient.from_transport(trans)
                print("Connected.")
                while True:
                    response = json.loads(await ws.recv())
                    # print("packet")

                    asyncio.create_task(process_response(response, ws, sftp))
    except asyncio.exceptions.TimeoutError:
        print("Timed out, retrying in 5 seconds.")
    finally:
        # user_task.cancel()
        trans.close()
        await asyncio.sleep(5)


async def process_response(response, ws, sftp):
    # Check if it's the response of a request.
    if "echo" in response.keys() and response["echo"] in pendingRecv.keys():
        echo = response["echo"]
        if response["status"] == "async":
            print(f"Task {echo} is async, not returning data.")
        elif response["status"] == "failed":
            print(f"Task {echo} failed, not returning data.")
            print(f"ERROR: {response['msg']}, {response['wording']}")
        pendingRecv[echo](response["data"])
        pendingRecv.pop(echo)

    # Check if it's an event.
    elif "post_type" in response.keys():
        post_type = response["post_type"]
        # Message event
        if post_type == "message":
            message = str(response["message"])

            match = re.match(r".*?\[CQ:image,.*?url=(.*?)].*", message)
            if match is not None:
                groups = match.groups()
                if len(groups) > 0:
                    image_record[response["user_id"]] = groups[0]
            else:
                if message.startswith("##"):
                    await process_command(ws, sftp, response, message)
                    # print("processed")
                elif message == "#help":
                    await say(ws, response, "<+> 用法：##[标签[#宽x高[#步数[#种子]]]] （方括号内为可选项）\n\n"
                                            "如果你的上一条消息是图片，会依据图片生成图像。支持图片分辨率识别。")

                image_record[response["user_id"]] = ""


async def process_command(ws, sftp, response, message):
    task = DrawTask(response)
    custom_res = False

    message = message[2::]
    parts = message.split("#")
    for i in range(0, len(parts)):
        # Positive prompt
        if i == 0:
            if parts[i] != "":
                task.prompt = parts[i]
        # Resolution
        elif i == 1:
            wh = parts[i].split("x")
            if len(wh) != 2 or (not wh[0].isdigit()) or (not wh[1].isdigit()) or int(
                    wh[0]) <= 0 or int(wh[1]) <= 0 or int(wh[0]) > 1024 or int(wh[1]) > 1024:
                err = "<!> 分辨率格式不对哦，应该像 512x512 这样~最大不超过1024x1024"
                await say(ws, response, err)
                return
            task.width = int(int(wh[0]) / 64) * 64
            task.height = int(int(wh[1]) / 64) * 64
            custom_res = True
        # Steps
        elif i == 2:
            if parts[i].isdigit() and int(parts[i]) <= 50:
                task.steps = int(parts[i])
            else:
                err = "<!> 步数应该是(0, 50]之间的整数！"
                await say(ws, response, err)
                return
        # Seed
        elif i == 3:
            if parts[i].isdigit():
                task.seed = int(parts[i])
            else:
                err = "<!> 种子应该是应该正整数。"
                await say(ws, response, err)
                return
    img = False
    if response["user_id"] in image_record.keys():
        link = image_record[response["user_id"]]
        if link != "":
            task.image = Image.open(BytesIO(requests.get(link).content))
            img = True
            if not custom_res:
                wid = task.image.width
                hig = task.image.height
                if max(wid, hig) > 1024:
                    factor = 1024 / max(wid, hig)
                    wid *= factor
                    hig *= factor
                task.width = int(wid / 64) * 64
                task.height = int(hig / 64) * 64
    drawQueue.append(task)
    queue = ""
    for item in drawQueue:
        card = ""
        if response["message_type"] == "group":
            data = await comm(ws, "get_group_member_info",
                              {"group_id": response["group_id"], "user_id": response["user_id"]})
            card = data["card"]
        else:
            card = response["sender"]["nickname"]
        queue += card + ", "
    queue = queue[:-2:]
    if not working:
        # asyncio.create_task(draw_queue(ws, sftp, response))
        threading.Thread(target=draw_queue, args=(ws, sftp, response)).start()
    await say(ws, response,
              f"<{'img2img' if img else 'txt2img'}> 成功创建任务！\n"
              f"当前队列：{queue}"
              )


async def comm(ws, action, params):
    data = {}

    def set_flag(d):
        nonlocal data
        data = d
        flag.set()

    global pendingRecv
    flag = asyncio.Event()
    if ws is None:
        print("WebSocket not initialized, cannot send request.")
        return
    echo = str(time.time_ns())
    req = {"action": action, "params": params, "echo": echo}
    await ws.send(json.dumps(req))
    pendingRecv[echo] = set_flag
    # print("wait")
    await flag.wait()
    # print("waited")
    return data


async def user(ws):
    def callback(data):
        print("sent")

    while True:
        await comm(ws, "send_msg",
                   {"message": input("Input message: "), "message_type": "private",
                    "user_id": 1347267108})  # , callback)


async def say_group(ws, message, group_id):
    await comm(ws, "send_msg", {"message": message, "message_type": "group", "group_id": group_id})


async def say_private(ws, message, user_id):
    await comm(ws, "send_msg", {"message": message, "message_type": "private", "user_id": user_id})


async def say(ws, response, message):
    if response["message_type"] == "group":
        await say_group(ws, message, response["group_id"])
    else:
        await say_private(ws, message, response["user_id"])
    # print("said")


def draw_queue(ws, sftp: paramiko.SFTPClient, response):
    global drawQueue, working
    working = True
    while len(drawQueue) > 0:
        task = drawQueue[0]
        print(f"Processing task from {response['sender']['nickname']}")
        negative = "lowres, bad anatomy, bad hands, text, error, missing fingers, extra digit, fewer digits, " \
                   "cropped, worst quality, low quality, normal quality, jpeg artifacts, signature, watermark, " \
                   "username, blurry, nude, nipple, vagina, nsfw, penis, anus"

        result = txt2img(task.prompt, negative, "", "", task.steps, 0, False, False, 1, 1, 11, task.seed,
                         -1, 0, 0, 0, False, task.height, task.width, False, False,
                         0, 0) if task.image is None else img2img(0, task.prompt, negative, "", "", task.image, None,
                                                                  None, None, 0, task.steps, 0, 0, 0, False, False, 1,
                                                                  1, 11, 0.75, task.seed, -1, 0, 0, 0, False,
                                                                  task.height, task.width, 0, False, 32, 0, "", "", 0)

        output_dir = "outputs/txt2img-images/setu.png"
        image = result[0][0]
        image.save(output_dir, format="PNG")

        sftp.put(output_dir, "/root/temp/setu.png")
        loop.create_task(say(ws, response,
                             f"[CQ:reply,id={task.response['message_id']}][CQ:image,file=file:///root/temp/setu.png]"))
        drawQueue.pop(0)
    working = False


if __name__ == "__main__":
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(network())
    # loop.run_forever()
