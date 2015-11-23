#coding:utf-8
import json, string, math  

def processTimeStamp(timestamp):
    hour = string.atoi(timestamp[8:10])
    if(hour < 19 and hour > 6):
        return 1 # day
    else: 
        return 0

def Json(input, output):
    fin = open(input, 'r')
    fout = open(output, 'w')
    js = json.load(fin) 
    results = js["results"]

    count1 = 0
    count0 = 0 
    pre_line = ''
    cur_line = ''

    for content in results:
        timestamp = content["timeStamp"]  # 20150419141301
        longi = content['longitude'] + 0.006259  # 121.40  121.468
        lati = content['latitude'] + 0.006328  # 31.01 31.045
        intensity = content["CdmaDbm"]

        time = processTimeStamp(timestamp)

        if (longi < 121.40 or longi > 121.47 or lati < 31.01 or lati > 31.045 or intensity < -99 or intensity > -44):
             continue

        cur_line = "%f %f %d %f" % (longi, lati, time, intensity) 
        if cur_line != pre_line:
            print >> fout, cur_line
        else:
            continue
        pre_line = cur_line


        if(time == 1):
            count1 = count1 + 1
        else:
            count0 = count0 + 1 

    fin.close()
    fout.close()
    print "0 have %d, 1 have %d" % (count0, count1)

def process(input, output):
    LONGTITUDE = 121.42187 + 0.006259
    LATITUDE = 31.015759 + 0.006328
    GAP = 0.001
    L = 25
    W = 16

    cnt = [0 for x in range(801)]
    s = [0 for x in range(801)]

    fin = open(input, 'r')
    fout = open(output, 'w')

    longi = 0.0
    lati = 0.0
    time = 0
    ss = 0

    for line in fin:
        longi, lati, time, ss = line.split()
        longi = float(longi)
        lati = float(lati)
        time = int(time)
        ss = float(ss)
        index = int((lati - LATITUDE) / GAP) * L + int((longi - LONGTITUDE) / GAP)
        #print "%f %f %d %f" % (longi, lati, time, ss)

        if (index > 400 or index < 0) :
            continue
        s[index + time * L * W] = s[index + time * L * W] + ss
        cnt[index + time * L * W] = cnt[index + time * L * W] + 1

    for index in range(0, 801):
        #print "cnt[%d] = %d" % (index, cnt[index])
        if (cnt[index] != 0):
            if index >= L*W:
                print >> fout, "%d %d %f" % (index - L*W, 1, s[index] / cnt[index])
            else:
                print >> fout, "%d %d %f" % (index, 0, s[index] / cnt[index])
Json("G3Data.json", 'raw.txt')
process("raw.txt", "data.txt")

