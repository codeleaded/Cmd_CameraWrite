#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define WIDTH 640
#define HEIGHT 480

unsigned char number = 0;

int main() {
    int fd = open("/dev/video10", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open video device");
        return 1;
    }

    struct v4l2_format format = {0};
    format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &format) < 0) {
        perror("Setting Pixel Format");
        close(fd);
        return 1;
    }

    // YUYV: 2 bytes per pixel
    size_t frame_size = WIDTH * HEIGHT * 2;
    unsigned char *frame = malloc(frame_size);

    // Einfaches Testbild: graues Bild mit waagerechtem Balken
    for (int i = 0; i < frame_size; i += 4) {
        frame[i]     = number; // Y1
        frame[i + 1] = number; // U
        frame[i + 2] = number; // Y2
        frame[i + 3] = number; // V
    }

    // Endlosschleife: immer wieder dasselbe Bild senden
    while (1) {
        number++;
        for (int i = 0; i < frame_size; i += 4) {
            frame[i]     = number; // Y1
            frame[i + 1] = number; // U
            frame[i + 2] = number; // Y2
            frame[i + 3] = number; // V
        }

        if (write(fd, frame, frame_size) < 0) {
            perror("Failed to write frame");
            break;
        }
        usleep(33000); // ca. 30 FPS
    }

    free(frame);
    close(fd);
    return 0;
}
