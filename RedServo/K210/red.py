import sensor, time, image
import utime
import lcd
from fpioa_manager import fm
from machine import UART
from Maix import GPIO  # 导入Maix库中的GPIO模块，用于控制GPIO引脚
import time
import uos

print("files:", uos.listdir("/flash"))

fm.register(13, fm.fpioa.GPIO0)     # 将引脚13映射为GPIO0
fm.register(12, fm.fpioa.GPIO1)     # 将引脚12映射为GPIO1
fm.register(14, fm.fpioa.GPIO2)     # 将引脚14映射为GPIO2
led_r = GPIO(GPIO.GPIO0, GPIO.OUT)  # 初始化GPIO0为输出模式，控制红色LED灯
led_g = GPIO(GPIO.GPIO1, GPIO.OUT)  # 初始化GPIO1为输出模式，控制绿色LED灯
led_b = GPIO(GPIO.GPIO2, GPIO.OUT)  # 初始化GPIO2为输出模式，控制蓝色LED灯

# 绑定UART2的引脚
fm.register(6, fm.fpioa.UART2_RX)   # 将引脚6绑定到UART2的接收引脚
fm.register(8, fm.fpioa.UART2_TX)   # 将引脚8绑定到UART2的发送引脚
# 初始化UART2对象
uart = UART(UART.UART2, 115200, 8, 0, 0, timeout=1000, read_buf_len=4096)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
#sensor.set_auto_exposure(1)
#sensor.set_auto_exposure(0, exposure=120000)
sensor.set_auto_gain(0, gain_db = 25)
sensor.set_auto_whitebal(0, rgb_gain_db = (0,0,255))
#sensor.set_brightness(0)   #设置亮度
#sensor.set_contrast(0) #对比度
sensor.set_vflip(True)
sensor.set_hmirror(False)
#sensor.set_windowing((224, 224))
sensor.skip_frames(time = 900)



lcd.init()
#lcd.init(invert=True)

flag = 0
leftup = (0, 0)
rightdown = (320, 240)

# led状态切换
def led(num):
    if num == 0:
        led_r.value(1)
        led_g.value(0)
        led_b.value(0)
    elif num == 1:
        led_r.value(0)
        led_g.value(1)
        led_b.value(0)
    elif num == 2:
        led_r.value(0)
        led_g.value(0)
        led_b.value(1)


def sendSaveData():
    print('初始化')
    with open("/flash/test.txt", "r") as f:
        data_str = f.read()
        print('data_str',data_str)

        segments = data_str.split(",")          # 按逗号分割字符串
        result = {}                             # 创建字典存储解析结果
        for segment in segments:                # 遍历每个键值对并解析
            key, value = segment.split(":")     # 按冒号分隔键值对
            result[key.strip()] = value.strip() # 去除空格并保存到字典中
        print('result',result)                  # 打印解析结果
        try:
            send_str = result['left_PWM']+','+result['up_PWM']+','+result['right_PWM']+','+\
            result['down_PWM']+','+result['K210_left']+','+result['K210_up']+','+\
            result['K210_right']+','+result['K210_down']+'\r\n'
            print('发送初始化数据')
            uart.write(send_str)  # 发送数据
            print(send_str+'发送完毕')
        except:
            print('发送初始化数据')
            uart.write('1610,1258,1273,1580,83,2,260,210\r\n')  # 发送数据
            print(send_str+'发送完毕111')



def SaveData(data_str):
    with open("/flash/test.txt", "w") as f:
        f.write(data_str)
        print('Save OK')

# 串口接收
def accept_uart():
    global flag, uart, lcd, sensor
    while True:
        img=sensor.snapshot()                           # 获取图片
        lcd.display(img)
        if uart.any():                                  # 判断是否有数据可读
            led(1)
            read_data = uart.read()                     # 读取数据
            #print("read_data =", read_data)
            #print(type(read_data))
            if read_data:
                try:
                    data_str = read_data.decode("utf-8")    # 将字节数据转换为字符串
                    segments = data_str.split(",")          # 按逗号分割字符串
                    result = {}                             # 创建字典存储解析结果
                    for segment in segments:                # 遍历每个键值对并解析
                        key, value = segment.split(":")     # 按冒号分隔键值对
                        result[key.strip()] = value.strip() # 去除空格并保存到字典中
                    print(result)                           # 打印解析结果
                    led(2)
                    flag = int(result['id'])
                    print('收到',flag)
                    if flag == 1:
                        flag = 0
                        print('flag == 1')
                        sendSaveData()
                    elif flag == 5:
                        print('flag == 5')
                        flag = 0
                        SaveData(data_str)
                    break
                except:
                    pass


# 识别激光确定位置
def pointPosition(num):
    global sensor, uart, lcd, leftup, rightdown
    flag0 = 0
    while flag0 == 0:
        print('识别激光确定位置中')
        sensor.set_framesize(sensor.QVGA)
        sensor.skip_frames(time = 300)
        img = sensor.snapshot()           # 获取图片
        roiQQ = (30,0,102,100)
        roiQ = (19,0,254,220)
        roice = ( 13, 0, 272, 221)
        for blobs in img.find_blobs([(80, 100, -7, 127, -7, 127)],roi = roiQ,
        x_stride = 1,y_stride = 1,pixels_threshold  = 10,
        area_threshold	=  10,
        merge = True, margin = 2):
            print(blobs)
            #if 100 > blobs.pixels():               # 找到面积最大的色块
            #img.draw_rectangle(blobs[0:4])
            img.draw_cross(blobs.cx(),blobs.cy(), color=255, size = 8)
            if num == 2:
                leftup = (blobs.cx(), blobs.cy())
            elif num == 3:
                rightdown = (blobs.cx(), blobs.cy())
            str2 = str(blobs.cx()) + ',' + str(blobs.cy()) + ',0,0,0,0,0,0\r\n'
            uart.write(str2)                        # 发送数据
            print(str2+'发送完毕')
            flag0 = 1
        lcd.display(img)

def shrink_rectangle(cx,cy,x0,y0,x1,y1,x2,y2,x3,y3):
    print(x0,y0,x1,y1,x2,y2,x3,y3)
    cx = (x0+x2)//2
    cy = (y0+y2)//2
    x0 = int(x0 + (cx - x0)*1/abs(x0-160)*5)
    y0 = int(y0 + (cy - y0)*y0/100*0.25)
    x1 = int(x1 + (cx - x1)*1/abs(x1-160)*6)
    y1 = int(y1 + (cy - y1)*y1/100*0.1)
    x2 = int(x2 + (cx - x2)*1/abs(x2-160)*5)
    y2 = int(y2 + (cy - y2)*y2/120*0.07)
    x3 = int(x3 + (cx - x3)*1/abs(x3-160)*5)
    y3 = int(y3 + (cy - y3)*y3/100*0.1)
    #print(x0,y0,x1,y1,x2,y2,x3,y3)
    return x0,y0,x1,y1,x2,y2,x3,y3



# A4边框识别
def A4Ti(num):
    global sensor, lcd, leftup, rightdown

    sensor.set_framesize(sensor.QQVGA)      #降低分辨率

    #sensor.set_brightness(10)                #设置亮度
    #sensor.set_contrast(5)                  #对比度

    #sensor.skip_frames(time = 500)
    sensor.set_auto_whitebal(0, rgb_gain_db = (0,0,0))
    with open("/flash/test.txt", "r") as f:
        data_str = f.read()
        segments = data_str.split(",")          # 按逗号分割字符串
        result = {}                             # 创建字典存储解析结果
        for segment in segments:                # 遍历每个键值对并解析
            key, value = segment.split(":")     # 按冒号分隔键值对
            result[key.strip()] = value.strip() # 去除空格并保存到字典中
        leftup = (int(result['K210_left']), int(result['K210_up']))
        rightdown = (int(result['K210_right']), int(result['K210_down']))
    print(leftup, rightdown)

    rects_list = []
    quit = 0
    #print(leftup[0],leftup[1],rightdown[0]-leftup[0],rightdown[1]-leftup[1])
    while quit == 0:
        img = sensor.snapshot()
        for r in img.find_rects(roi = (leftup[0]//2-2,leftup[1]//2,
        (rightdown[0]-leftup[0])//2+4,(rightdown[1]-leftup[1])//2),threshold =10000):
            rects = r.corners()
            cx = r.x()+r.w()//2         # center
            cy = r.y()+r.h()//2
            print('cx,cy',cx*2,cy*2)
            # 序列解包
            x0,y0,x1,y1,x2,y2,x3,y3=rects[3][0]*2, rects[3][1]*2, rects[2][0]*2, rects[2][1]*2,rects[1][0]*2, rects[1][1]*2, rects[0][0]*2, rects[0][1]*2
            if num == 3:
                if abs((x1-x0)-(x2-x3)) < 10:
                    quit = 1
                    str3 = str(x0+1)+','+str(y0)+','+str(x1-2)+','+str(y1+1)+','+str(x2-2)\
                    +','+str(y2-3)+','+str(x3+3)+','+str(y3-3)+'\r\n'
                    uart.write(str3)  # 发送数据
                    print(str3+'Ti3发送完毕')
            if num == 4:
                if abs((x1-x0)-(x2-x3)) < 10:
                    quit = 1
                    if y0 <= y1:
                        x0,y0,x1,y1,x2,y2,x3,y3 = shrink_rectangle(cx*2,cy*2,x0,y0,x1,y1,x2,y2,x3,y3)
                    elif y0 > y1:
                        x0,y0,x1,y1,x2,y2,x3,y3 = shrink_rectangle(cx*2,cy*2,x1,y1,x2,y2,x3,y3,x0,y0)
                    str3 = str(x0)+','+str(y0)+','+str(x1)+','+str(y1)+','+str(x2)\
                    +','+str(y2)+','+str(x3)+','+str(y3)+'\r\n'
                    uart.write(str3)  # 发送数据
                    print(str3+'Ti4发送完毕')
            for p in r.corners():
                img.draw_circle(p[0], p[1], 5, color = (0, 255, 0))
        lcd.display(img)


while True:
    led(0)
    img=sensor.snapshot()                           # 获取图片
    lcd.display(img)

    #flag = 4
    accept_uart()                                   # 串口接受

    if flag == 2:
        print('进入2')
        flag = 0
        pointPosition(2)
    if flag == 3:
        print('进入3')
        flag = 0
        pointPosition(3)
    if flag == 4:
        print('进入第三题')
        flag = 0
        A4Ti(3)
    if flag == 6:
        print('进入第四题')
        flag = 0
        A4Ti(4)



