import matplotlib.pyplot as plt
import numpy as np
from utils.angle import angle_mod

x = []
y = []
yaw = []
direction = []

with open('path.txt', 'r') as f:
    for point in f.readlines():
        a, b, c, d = eval(point)
        x.append(a)
        y.append(b)
        yaw.append(c)
        direction.append(d)
    f.close()

'''
plt.subplot(211)
plt.plot(x, y, 'r')
plt.subplot(212)
plt.plot(np.rad2deg(yaw))
plt.grid(True)
plt.show()
'''
"""
number_of_points = len(x)
segment_length = []

for i in range(number_of_points-1):
    x1 = x[i]; y1 = y[i]
    x2 = x[i+1]; y2 = y[i+1]
    segment_length.append(np.hypot(abs(x1-x2), abs(y1-y2)))

plt.subplot(211)
plt.plot(segment_length)

deleted = 0
for i, d in enumerate(segment_length):
    if d < 0.35:
        x.pop(i-deleted)
        y.pop(i-deleted)
        yaw.pop(i-deleted)
        deleted += 1
"""
number_of_points = len(x)
segment_length = []

for i in range(number_of_points-1):
    x1 = x[i]; y1 = y[i]
    x2 = x[i+1]; y2 = y[i+1]
    segment_length.append(np.hypot(abs(x1-x2), abs(y1-y2)))

plt.plot(segment_length)
#plt.show()

plt.figure()
#plt.subplot(211)
#plt.plot(x, y)
#plt.axis('equal')
plt.subplot(212)
plt.plot(np.rad2deg(yaw))
plt.grid(True)
plt.title("Yaw (degree)")
#plt.show()


window = 2
vel = []
for i in range(len(x)):
    if i + window <= len(x):
        yaw_window = yaw[i:i+window]
    else:
        yaw_window = yaw[i:]
    d = 0
    for j, _ in enumerate(yaw_window):
        if j < len(yaw_window) - 1:
            d += angle_mod(abs(yaw_window[j] - yaw_window[j + 1])) ** 2
    vel.append(15 - 40 * np.sqrt(d))
for i in range(5):
    vel[-(i+2)] = i
vel[-1] = 0

plt.subplot(211)
plt.plot(vel)
plt.title("Speed (m/s)")
plt.grid(True)
plt.show()

print(len(x))
print(len(vel))
