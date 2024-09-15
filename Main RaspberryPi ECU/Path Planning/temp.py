import matplotlib.pyplot as plt

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

print(x)
print(y)
print(yaw)
print(direction)


ox = []
oy = []
"""
with open('map.txt', 'r') as f:
    for point in f.readlines():
        a, b = eval(point)
        ox.append(a)
        oy.append(b)
    f.close()
"""
"""
for i in range(265, 309):
    if not 278 <= i <= 297:
        ox.append(i)
        oy.append(178)
    ox.append(i)
    oy.append(240)
for i in range(178, 241):
    ox.append(308)
    oy.append(i)
    ox.append(265)
    oy.append(i)

for i in range(188, 237):
    ox.append(277)
    oy.append(i)
    ox.append(279)
    oy.append(i)
    ox.append(295)
    oy.append(i)
    ox.append(297)
    oy.append(i)

for i in range(272, 278):
    for j in range(188, 237, 3):
        ox.append(i)
        oy.append(j)
for i in range(279, 285):
    for j in range(188, 237, 3):
        ox.append(i)
        oy.append(j)
for i in range(290, 296):
    for j in range(188, 237, 3):
        ox.append(i)
        oy.append(j)
for i in range(297, 303):
    for j in range(188, 237, 3):
        ox.append(i)
        oy.append(j)
"""
for i in range(0, 44):
    if not 13 <= i <= 32:
        ox.append(i)
        oy.append(0)
    ox.append(i)
    oy.append(62)
for i in range(0, 63):
    ox.append(43)
    oy.append(i)
    ox.append(0)
    oy.append(i)

for i in range(10, 59):
    ox.append(12)
    oy.append(i)
    ox.append(14)
    oy.append(i)
    ox.append(30)
    oy.append(i)
    ox.append(32)
    oy.append(i)

for i in range(7, 13):
    for j in range(10, 59, 3):
        ox.append(i)
        oy.append(j)
for i in range(14, 20):
    for j in range(10, 59, 3):
        ox.append(i)
        oy.append(j)
for i in range(25, 31):
    for j in range(10, 59, 3):
        ox.append(i)
        oy.append(j)
for i in range(32, 38):
    for j in range(10, 59, 3):
        ox.append(i)
        oy.append(j)

plt.plot(ox, oy, 'sk')
plt.grid(True)
plt.axis('equal')
plt.show()
