import sensor, time, image
import utime
import lcd
from fpioa_manager import fm
from machine import UART
from Maix import GPIO  # 导入Maix库中的GPIO模块，用于控制GPIO引脚
import time
import uos


fm.register(27, fm.fpioa.GPIO0)     # 将引脚13映射为GPIO0
fm.register(28, fm.fpioa.GPIO1)     # 将引脚12映射为GPIO1
fm.register(29, fm.fpioa.GPIO2)     # 将引脚14映射为GPIO2
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
sensor.set_framesize(sensor.QVGA)
#sensor.set_auto_exposure(1)
#sensor.set_auto_exposure(0, exposure=120000)
sensor.set_auto_gain(0, gain_db = 5)
sensor.set_auto_whitebal(0, rgb_gain_db = (0,0,0))
#sensor.set_brightness(0)   #设置亮度
#sensor.set_contrast(3) #对比度
sensor.set_vflip(True)
sensor.set_hmirror(True)
#sensor.set_windowing((224, 224))
sensor.skip_frames(time = 900)

lcd.init()

flag = 0

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


# 串口接收
def accept_uart():
    global flag, uart, lcd, sensor
    #while True:
    #img=sensor.snapshot()                           # 获取图片
    #lcd.display(img)
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
                return 1
                #break
            except:
                pass
    return 0


# 识别激光确定位置
def pointPosition():
    global sensor, uart, lcd
    flag0 = 0
    #sensor.set_brightness(-3)   #设置亮度
    #sensor.set_contrast(3) #对比度
    #sensor.set_auto_exposure(False,500)  #曝光速度
    while flag0 == 0:
        print('识别激光确定位置中')
        flag0 = accept_uart()
        img = sensor.snapshot()           # 获取图片
        red_x = 0

        red_y = 0
        green_x = 0
        green_y = 0
        #,(80, 100, -11, 0, 0, 10)
        for blobs in img.find_blobs([(50, 100, 10, 127, -20, 127)],
        x_stride = 1,y_stride = 1,pixels_threshold  = 10,
        area_threshold	=  10,
        merge = True, margin = 1):
            #if 50 < blobs.pixels():               # 找到面积最大的色块
            #print(blobs)
            #if blobs.code() == 1:
            red_x = blobs.x()
            red_y = blobs.y()
            img.draw_cross(blobs.cx(),blobs.cy(), color=(0,0,255), size = 8)
        for blobs in img.find_blobs([(70, 100, -50, 5, -10, 20)],
        x_stride = 5,y_stride = 5,pixels_threshold  = 10,
        area_threshold	=  10,
        merge = True, margin = 1):
            #if 100 < blobs.pixels():               # 找到面积最大的色块
            #print(blobs)
            #if blobs.code() == 1:
                #red_x = blobs.x()
                #red_y = blobs.y()
                #img.draw_cross(blobs.cx(),blobs.cy(), color=(0,0,255), size = 8)
            #if blobs.code() == 2:
            green_x = blobs.x()
            green_y = blobs.y()
            img.draw_cross(blobs.cx(),blobs.cy(), color=(255,0,0), size = 8)
        utime.sleep_ms(20)
        if red_x*red_y*green_x*green_y != 0:
            str2 = str(red_x)+','+str(red_y)+','+str(green_x)+','+str(green_y)+',0,0,0,0\r\n'
            utime.sleep_ms(50)
            uart.write(str2)                        # 发送数据
            print(str2+'发送完毕')
        lcd.display(img)


def Fing_Write():
    global sensor
    flag0 = 0
    while flag0 == 0:
        img=sensor.snapshot()
        for blobs in img.find_blobs([(60, 100, -17, 127, -6, 127)],
        x_stride = 100,y_stride = 100,pixels_threshold = 15000, area_threshold = 15000,
        merge = True, margin = 200):
            if abs(blobs.w()-blobs.h()) > 50:
                continue
            img.draw_rectangle(blobs[0:4])
            #print(blobs)
            str2 = str(blobs.cx()) + ',' + str(blobs.cy()) + ',0,0,0,0,0,0\r\n'
            uart.write(str2)                        # 发送数据
            print(str2+'发送完毕')
            img.draw_cross(blobs.cx(),blobs.cy(), color=255, size = 8)
            lcd.display(img)
            flag0 = 1
        lcd.display(img)




while True:
    img=sensor.snapshot()                  # 获取图片
    lcd.display(img)
    flag = 2
    #accept_uart()

    if flag == 1:
        flag = 0
        Fing_Write()
    if flag == 2:
        flag = 0
        pointPosition()



    #led(0)
    #img=sensor.snapshot()                           # 获取图片
    #lcd.display(img)

    #flag = 4
    #accept_uart()                                   # 串口接受

    #if flag == 2:
        #print('进入2')
        #flag = 0
        #pointPosition(2)
    #if flag == 3:
        #print('进入3')
        #flag = 0
        #pointPosition(3)
    #if flag == 4:
        #print('进入第三题')
        #flag = 0
        #A4Ti(3)
    #if flag == 6:
        #print('进入第四题')
        #flag = 0
        #A4Ti(4)
    #pointPosition()
