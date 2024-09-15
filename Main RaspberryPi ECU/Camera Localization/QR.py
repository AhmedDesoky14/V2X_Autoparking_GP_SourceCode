import cv2 as cv
import numpy as np
import sys


def read_camera_parameters(filepath='intrinsic.dat'):
    inf = open(filepath, 'r')

    cmtx = []
    dist = []

    # ignore first line
    line = inf.readline()
    for _ in range(3):
        line = inf.readline().split()
        line = [float(en) for en in line]
        cmtx.append(line)

    # ignore line that says "distortion"
    line = inf.readline()
    line = inf.readline().split()
    line = [float(en) for en in line]
    dist.append(line)

    # cmtx = camera matrix, dist = distortion parameters
    return np.array(cmtx), np.array(dist)


def get_qr_coords(cmtx, dist, points):
    # Selected coordinate points for each corner of QR code.
    qr_edges = np.array([[0, 0, 0],
                         [0, 5.8, 0],
                         [5.8, 5.8, 0],
                         [5.8, 0, 0]], dtype='float32').reshape((4, 1, 3))

    # determine the orientation of QR code coordinate system with respect to camera coorindate system.
    ret, rvec, tvec = cv.solvePnP(qr_edges, points, cmtx, dist)
    print("Origin\n", points[0])

    # Define unit xyz axes. These are then projected to camera view using the rotation matrix and translation vector.
    unitv_points = np.array([[0, 0, 0], [5.8, 0, 0], [0, 5.8, 0], [0, 0, 5.8]], dtype='float32').reshape((4, 1, 3))
    if ret:
        points, jac = cv.projectPoints(unitv_points, rvec, tvec, cmtx, dist)
        print("Projected Origin\n", points[0])
        return points, rvec, tvec

    # return empty arrays if rotation and translation values not found
    else:
        return [], [], []


def show_axes(cmtx, dist, in_source):
    cap = cv.VideoCapture(in_source)

    qr = cv.QRCodeDetector()

    #_, img = cap.read()
    #result = cv.VideoWriter('QR.avi',
    #                        cv.VideoWriter_fourcc(*'MJPG'),
    #                        10, (img.shape[1], img.shape[0]))
    while True:
        ret, img = cap.read()
        if not ret:
            break

        ret_qr, points = qr.detect(img)

        if ret_qr:
            data, _ = qr.decode(img, points)
            print(data)
            axis_points, rvec, tvec = get_qr_coords(cmtx, dist, points)
            rmat, _ = cv.Rodrigues(rvec)
            t = np.hstack((np.vstack((rmat, [0, 0, 0])), np.vstack((tvec, [1]))))
            print("Transformation:\n", t)

            # BGR color format
            colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (0, 0, 0)]

            # check axes points are projected to camera view.
            if len(axis_points) > 0:
                axis_points = axis_points.reshape((4, 2))

                origin = (int(axis_points[0][0]), int(axis_points[0][1]))

                for p, c in zip(axis_points[1:], colors[:3]):
                    p = (int(p[0]), int(p[1]))

                    # Sometimes qr detector will make a mistake and projected point will overflow integer value. We skip these cases.
                    if origin[0] > 5*img.shape[1] or origin[1] > 5*img.shape[1]:
                        break
                    if p[0] > 5*img.shape[1] or p[1] > 5*img.shape[1]:
                        break

                    cv.line(img, origin, p, c, 5)

        cv.imshow('frame', img)
        #result.write(img)
        k = cv.waitKey(10)
        if k == 27:
            break  # 27 is ESC key.

    cap.release()
    #result.release()
    cv.destroyAllWindows()


if __name__ == '__main__':
    # read camera intrinsic parameters.
    cmtx, dist = read_camera_parameters()

    input_source = 'test3.mp4'
    if len(sys.argv) > 1:
        input_source = int(sys.argv[1])

    show_axes(cmtx, dist, input_source)
