import cv2 as cv
import numpy as np


def read_camera_parameters(filepath='intrinsic.dat'):
    inf = open(filepath, 'r')

    cmtx = []
    dist = []

    # ignore first line
    inf.readline()
    for _ in range(3):
        line = inf.readline().split()
        line = [float(en) for en in line]
        cmtx.append(line)

    # ignore line that says "distortion"
    inf.readline()
    line = inf.readline().split()
    line = [float(en) for en in line]
    dist.append(line)

    # cmtx = camera matrix, dist = distortion parameters
    return np.array(cmtx), np.array(dist)


cmtx, dist = read_camera_parameters()

img = cv.imread('Screenshot_2.png')
# img = cv.undistort(img, cmtx, dist)

qr = cv.QRCodeDetector()
ret, points = qr.detect(img)
data = 0

if ret:
    data, _ = qr.decode(img, points)
    data = eval(data)
points = points.reshape((4, 2))
origin = (int(points[0][0]), int(points[0][1]))
p1 = (int(points[1][0]), int(points[1][1]))
p2 = (int(points[2][0]), int(points[2][1]))
p3 = (int(points[3][0]), int(points[3][1]))

print(data)
print(origin)

qr_width = 580 / 2
qr_height = 580 / 2
output_width = 1000
output_height = 1000

dst_pts = np.array([[output_width // 2 - qr_width // 2, output_height // 2 - qr_height // 2],
                    [output_width // 2 + qr_width // 2, output_height // 2 - qr_height // 2],
                    [output_width // 2 + qr_width // 2, output_height // 2 + qr_height // 2],
                    [output_width // 2 - qr_width // 2, output_height // 2 + qr_height // 2]], dtype=np.float32)

M = cv.getPerspectiveTransform(points, dst_pts)
print(M)

corrected_img = cv.warpPerspective(img, M, (output_width, output_height))

ret, points = qr.detect(corrected_img)
if ret:
    origin = tuple(dst_pts[0].astype('int'))
    p1 = tuple(dst_pts[1].astype('int'))
    p2 = tuple(dst_pts[2].astype('int'))
    p3 = tuple(dst_pts[3].astype('int'))
    data, _ = qr.decode(corrected_img, points)
    cv.line(corrected_img, origin, p3, (255, 0, 0), 5)
    cv.line(corrected_img, origin, p1, (0, 255, 0), 5)
    cv.line(corrected_img, origin, origin, (0, 0, 255), 5)
    print(data)

M_inv = cv.invert(M)[1]
print(M_inv)
undistorted = cv.warpPerspective(corrected_img, M_inv, img.shape[:2][::-1])

cv.imshow('img', corrected_img)
cv.waitKey()

cv.imshow('img', undistorted)
cv.waitKey()

# cv.line(img, origin, p1, (255, 0, 0), 5)
cv.imshow('img', img)
cv.waitKey()
