#!/usr/bin/python
'''import os
import glob
import csv
import xlwt # from http://www.python-excel.org/

for csvfile in glob.glob(os.path.join('.', '*.csv')):
    wb = xlwt.Workbook()
    ws = wb.add_sheet('data')
    with open(csvfile, 'rb') as f:
        reader = csv.reader(f)
        for r, row in enumerate(reader):
            for c, val in enumerate(row):
                ws.write(r, c, val)
    wb.save(csvfile + '.xls')      #code to create individual csv files donot touch'''



import os
import platform
import glob
import csv
import xlwt
import math
import numpy as np
import matplotlib.pyplot as plt



x86pocl_name = []
x86pocl_value = []
armpocl_name = []
armpocl_value = []

search_for = ['logic']

for csvfile in glob.glob(os.path.join('./x86_64_c/', '*.csv')):
    fpath = csvfile.split("/x86_64_c/", 1)
    fname = fpath[1].split(".", 1) ## fname[0] should be our worksheet name

    with open(csvfile, 'rb') as inf:
        reader = csv.reader(inf)
        for row in reader:
            if row[0] in search_for:
                x86pocl_name.append(fname[0])
		x86pocl_value.append(int(row[1]))

for csvfile in glob.glob(os.path.join('./armv7l_c/', '*.csv')):
    fpath = csvfile.split("/armv7l_c/", 1)
    fname = fpath[1].split(".", 1) ## fname[0] should be our worksheet name

    with open(csvfile, 'rb') as inf:
        reader = csv.reader(inf)
        for row in reader:
            if row[0] in search_for: 
		armpocl_name.append(fname[0])
		armpocl_value.append(int(row[1]))
		

print(x86pocl_name)
print(x86pocl_value)

print(armpocl_name)
print(armpocl_value)

N = int(len(x86pocl_name)) 
offset = [1] * N




armpocl_value = [math.log(x,1.0005) for x in armpocl_value]
x86pocl_value = [math.log(x,1.0005) for x in x86pocl_value]


ind = np.arange(0,N*2,2)  # the x locations for the groups
width = 0.30     # the width of the bars

fig, ax = plt.subplots(figsize=(17,10))
rects1 = ax.bar(ind, x86pocl_value, width, color='r', yerr=offset)
rects2 = ax.bar(ind+width, armpocl_value, width, color='y', yerr=offset)


# add some
ax.set_ylabel('Time(usec) represented as log10()')
ax.set_title('Timing analysis for c logic')
ax.set_xticks(ind+width)
ax.set_xticklabels( x86pocl_name )

ax.legend( (rects1[0], rects2[0]), ('x86_64', 'armv7l') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/1., 0.99*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
#plt.show()
plt.savefig('comp_c.png')

        
		

      


