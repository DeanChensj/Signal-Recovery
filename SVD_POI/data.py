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
Json("G3Data.json", 'raw.txt')

